#include "StencilGenerator.hpp"
#include <QDebug>
#include <QElapsedTimer>
#include <QtConcurrent/QtConcurrent>
#include <cmath>

/**
 * @brief Constructor for StencilGenerator
 * @param parent Parent QObject
 */
StencilGenerator::StencilGenerator(QObject *parent) : QObject(parent) {
    qDebug() << "StencilGenerator initialized";
}

/**
 * @brief Destructor for StencilGenerator
 */
StencilGenerator::~StencilGenerator() {
    qDebug() << "StencilGenerator destroyed";
}

/**
 * @brief Load image from file path
 * @param filePath Path to image file
 * @return true if loaded successfully
 */
bool StencilGenerator::loadImage(const QString &filePath) {
    try {
        originalImage_ = cv::imread(filePath.toStdString(), cv::IMREAD_COLOR);
        if (originalImage_.empty()) {
            qWarning() << "Failed to load image from:" << filePath;
            return false;
        }
        
        // Convert BGR to RGB for Qt compatibility
        cv::cvtColor(originalImage_, originalImage_, cv::COLOR_BGR2RGB);
        
        qDebug() << "Image loaded:" << filePath 
                 << "Size:" << originalImage_.cols << "x" << originalImage_.rows
                 << "Channels:" << originalImage_.channels();
        
        return true;
    } catch (const cv::Exception &e) {
        qCritical() << "OpenCV error loading image:" << e.what();
        return false;
    }
}

/**
 * @brief Load image from QImage
 * @param image Qt QImage to load
 * @return true if loaded successfully
 */
bool StencilGenerator::loadImage(const QImage &image) {
    if (image.isNull()) {
        qWarning() << "Attempted to load null QImage";
        return false;
    }
    
    originalImage_ = qImageToCvMat(image);
    qDebug() << "QImage loaded, converted to CV Mat";
    return true;
}

/**
 * @brief Load image from OpenCV Mat
 * @param image OpenCV Mat to load
 * @return true if loaded successfully
 */
bool StencilGenerator::loadImage(const cv::Mat &image) {
    if (image.empty()) {
        qWarning() << "Attempted to load empty CV Mat";
        return false;
    }
    
    image.copyTo(originalImage_);
    return true;
}

/**
 * @brief Preprocess image before stencil generation
 * @param image Input image
 * @param params Processing parameters
 * @return Preprocessed image
 */
cv::Mat StencilGenerator::preprocessImage(const cv::Mat &image, const StencilParams &params) {
    cv::Mat processed = image.clone();
    
    // Convert to grayscale if not already
    if (processed.channels() == 3) {
        processed = convertToGrayscale(processed);
    }
    
    // Apply brightness and contrast adjustment
    if (params.brightness != 0.0f || params.contrast != 1.0f) {
        processed = adjustBrightnessContrast(processed, params.contrast, params.brightness);
    }
    
    // Apply blur if specified
    if (params.blurRadius > 0) {
        if (params.preserveEdges) {
            // Bilateral filter preserves edges while reducing noise
            processed = applyBilateralFilter(processed, params.blurRadius * 2 + 1, 
                                           75, 75);
        } else {
            processed = applyGaussianBlur(processed, params.blurRadius);
        }
    }
    
    return processed;
}

/**
 * @brief Generate stencil with current parameters
 * @param params Processing parameters
 * @return StencilResult containing processed image and metadata
 */
StencilResult StencilGenerator::generateStencil(const StencilParams &params) {
    StencilResult result;
    QElapsedTimer timer;
    timer.start();
    
    emit processingStarted();
    
    try {
        if (!hasImage()) {
            result.errorMessage = "No image loaded";
            emit processingError(result.errorMessage);
            return result;
        }
        
        cv::Mat processed;
        cv::Mat preprocessed = preprocessImage(originalImage_, params);
        
        emit processingProgress(30);
        
        // Apply selected processing mode
        switch (params.mode) {
            case ProcessingMode::SIMPLE_THRESHOLD:
                processed = applySimpleThreshold(preprocessed, params);
                break;
            case ProcessingMode::EDGE_DETECTION:
                processed = applyEdgeDetection(preprocessed, params);
                break;
            case ProcessingMode::ADAPTIVE_THRESHOLD:
                processed = applyAdaptiveThreshold(preprocessed, params);
                break;
            case ProcessingMode::MULTI_LAYER:
                processed = applyMultiLayer(preprocessed, params);
                break;
            case ProcessingMode::CONTOUR_POLYGON:
                processed = applyContourPolygon(preprocessed, params);
                break;
            case ProcessingMode::DETAIL_PRESERVING:
                processed = applyDetailPreserving(preprocessed, params);
                break;
            default:
                processed = applySimpleThreshold(preprocessed, params);
                break;
        }
        
        emit processingProgress(70);
        
        // Invert colors if requested
        if (params.invertColors) {
            cv::bitwise_not(processed, processed);
        }
        
        // Detect and bridge floating islands for stencil applications
        if (params.mode != ProcessingMode::CONTOUR_POLYGON) {
            cv::Mat islands = detectFloatingIslands(processed);
            if (!islands.empty()) {
                // If islands detected, bridge them
                processed = autoBridgeIslands(processed, 3, 255);
                result.islandCount = cv::countNonZero(islands);
            }
        }
        
        // Resize if output dimensions specified
        if (params.outputWidth > 0 && params.outputHeight > 0) {
            cv::resize(processed, processed, 
                      cv::Size(params.outputWidth, params.outputHeight),
                      params.maintainAspectRatio ? cv::INTER_AREA : cv::INTER_LINEAR);
        }
        
        // Store results
        processed.copyTo(result.processedMat);
        result.fullResolutionImage = cvMatToQImage(processed);
        
        // Find contours for polygon mode
        if (params.mode == ProcessingMode::CONTOUR_POLYGON) {
            result.contours = findContours(processed);
            qDebug() << "Found" << result.contours.size() << "contours";
        }
        
        // Calculate statistics
        cv::Mat binary;
        cv::threshold(processed, binary, 127, 255, cv::THRESH_BINARY);
        result.blackPixels = cv::countNonZero(binary == 0);
        result.whitePixels = cv::countNonZero(binary == 255);
        
        result.success = true;
        result.processingTimeMs = timer.elapsed();
        
        qDebug() << "Stencil generated in" << result.processingTimeMs << "ms"
                 << "Black:" << result.blackPixels << "White:" << result.whitePixels;
        
        emit processingProgress(100);
        emit processingCompleted(result);
        
    } catch (const cv::Exception &e) {
        result.errorMessage = QString("OpenCV error: %1").arg(e.what());
        result.success = false;
        emit processingError(result.errorMessage);
        qCritical() << result.errorMessage;
    } catch (const std::exception &e) {
        result.errorMessage = QString("Error: %1").arg(e.what());
        result.success = false;
        emit processingError(result.errorMessage);
        qCritical() << result.errorMessage;
    }
    
    return result;
}

/**
 * @brief Generate live preview for real-time updates
 * @param params Processing parameters
 * @param maxPreviewSize Maximum size for preview (maintains aspect ratio)
 * @return StencilResult with preview image
 */
StencilResult StencilGenerator::generateLivePreview(const StencilParams &params, int maxPreviewSize) {
    StencilResult result;
    
    try {
        if (!hasImage()) {
            return result;
        }
        
        // Create a smaller copy for preview
        cv::Mat previewImage = resizeImage(originalImage_, maxPreviewSize);
        
        // Apply preprocessing
        cv::Mat preprocessed = preprocessImage(previewImage, params);
        
        // Apply processing (simplified for speed)
        cv::Mat processed;
        switch (params.mode) {
            case ProcessingMode::SIMPLE_THRESHOLD:
                processed = applySimpleThreshold(preprocessed, params);
                break;
            case ProcessingMode::EDGE_DETECTION:
                processed = applyEdgeDetection(preprocessed, params);
                break;
            default:
                // For preview, use simple threshold for speed
                cv::threshold(preprocessed, processed, params.threshold, 255, cv::THRESH_BINARY);
                break;
        }
        
        // Convert to QImage for display
        result.previewImage = cvMatToQImage(processed);
        result.success = true;
        
    } catch (...) {
        // Silently fail for preview
    }
    
    return result;
}

/**
 * @brief Apply simple threshold processing
 * @param image Input grayscale image
 * @param params Processing parameters
 * @return Thresholded binary image
 */
cv::Mat StencilGenerator::applySimpleThreshold(const cv::Mat &image, const StencilParams &params) {
    cv::Mat result;
    cv::threshold(image, result, params.threshold, 255, cv::THRESH_BINARY);
    return result;
}

/**
 * @brief Apply edge detection (wireframe mode)
 * @param image Input grayscale image
 * @param params Processing parameters
 * @return Edge-detected image
 */
cv::Mat StencilGenerator::applyEdgeDetection(const cv::Mat &image, const StencilParams &params) {
    cv::Mat edges, result;
    
    // Apply Canny edge detection
    cv::Canny(image, edges, params.edgeLowThreshold, 
              params.edgeHighThreshold, params.edgeKernelSize);
    
    // Invert edges to get black lines on white background
    cv::bitwise_not(edges, result);
    
    return result;
}

/**
 * @brief Apply adaptive thresholding
 * @param image Input grayscale image
 * @param params Processing parameters
 * @return Adaptively thresholded image
 */
cv::Mat StencilGenerator::applyAdaptiveThreshold(const cv::Mat &image, const StencilParams &params) {
    cv::Mat result;
    
    // Ensure block size is odd
    int blockSize = params.adaptiveBlockSize;
    if (blockSize % 2 == 0) blockSize++;
    
    cv::adaptiveThreshold(image, result, 255, 
                         cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                         cv::THRESH_BINARY,
                         blockSize,
                         params.adaptiveC);
    
    return result;
}

/**
 * @brief Apply multi-layer stencil processing
 * @param image Input grayscale image
 * @param params Processing parameters
 * @return Multi-layer stencil image
 */
cv::Mat StencilGenerator::applyMultiLayer(const cv::Mat &image, const StencilParams &params) {
    cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);
    
    // Calculate layer thresholds
    int layerStep = 255 / params.layerCount;
    
    for (int i = 0; i < params.layerCount; i++) {
        int lowThresh = i * layerStep;
        int highThresh = (i + 1) * layerStep;
        
        cv::Mat layer;
        cv::inRange(image, lowThresh, highThresh, layer);
        
        // Assign different gray levels to each layer
        int grayValue = 255 - (i * (255 / params.layerCount));
        result.setTo(grayValue, layer);
    }
    
    return result;
}

/**
 * @brief Apply contour polygon simplification
 * @param image Input grayscale image
 * @param params Processing parameters
 * @return Polygon simplified image
 */
cv::Mat StencilGenerator::applyContourPolygon(const cv::Mat &image, const StencilParams &params) {
    // First, create binary image
    cv::Mat binary;
    cv::threshold(image, binary, params.threshold, 255, cv::THRESH_BINARY);
    
    // Remove small noise
    binary = removeSmallNoise(binary, params.minContourArea);
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binary, contours, hierarchy, 
                     cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    
    // Create blank output image
    cv::Mat result = cv::Mat::zeros(binary.size(), CV_8UC3);
    
    // Simplify and draw contours
    for (size_t i = 0; i < contours.size(); i++) {
        if (contours[i].empty()) continue;
        
        // Calculate contour area
        double area = cv::contourArea(contours[i]);
        if (area < params.minContourArea) continue;
        
        // Simplify polygon
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contours[i], approx, params.polygonEpsilon, true);
        
        // Draw filled polygon
        cv::Scalar color = (hierarchy[i][3] < 0) ? cv::Scalar(255, 255, 255) :  // Outer contour
                                                     cv::Scalar(0, 0, 0);      // Hole
        
        cv::drawContours(result, std::vector<std::vector<cv::Point>>{approx}, 0, 
                        color, cv::FILLED);
    }
    
    // Convert to grayscale
    cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
    
    return result;
}

/**
 * @brief Apply detail preserving threshold
 * @param image Input grayscale image
 * @param params Processing parameters
 * @return Detail-preserving binary image
 */
cv::Mat StencilGenerator::applyDetailPreserving(const cv::Mat &image, const StencilParams &params) {
    cv::Mat result;
    
    // Apply CLAHE for local contrast enhancement
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(cv::Size(8, 8));
    
    cv::Mat enhanced;
    clahe->apply(image, enhanced);
    
    // Combine global and adaptive threshold
    cv::Mat global;
    cv::threshold(enhanced, global, params.threshold, 255, cv::THRESH_BINARY);
    
    cv::Mat adaptive;
    cv::adaptiveThreshold(enhanced, adaptive, 255, 
                         cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                         cv::THRESH_BINARY, 11, 2);
    
    // Blend both methods
    cv::addWeighted(global, 0.7, adaptive, 0.3, 0, result);
    
    return result;
}

/**
 * @brief Convert OpenCV Mat to Qt QImage
 * @param mat OpenCV Mat to convert
 * @return QImage
 */
QImage StencilGenerator::cvMatToQImage(const cv::Mat &mat) {
    try {
        if (mat.empty()) {
            return QImage();
        }
        
        // Convert based on number of channels
        switch (mat.type()) {
            case CV_8UC1: {  // Grayscale
                QImage image(mat.data, mat.cols, mat.rows, 
                            static_cast<int>(mat.step), QImage::Format_Grayscale8);
                return image.copy();  // Copy to ensure data persistence
            }
            case CV_8UC3: {  // RGB
                cv::Mat rgb;
                cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
                QImage image(rgb.data, rgb.cols, rgb.rows, 
                            static_cast<int>(rgb.step), QImage::Format_RGB888);
                return image.copy();
            }
            case CV_8UC4: {  // RGBA
                QImage image(mat.data, mat.cols, mat.rows, 
                            static_cast<int>(mat.step), QImage::Format_RGBA8888);
                return image.copy();
            }
            default: {
                // Convert to 8-bit
                cv::Mat converted;
                mat.convertTo(converted, CV_8UC(mat.channels()));
                return cvMatToQImage(converted);
            }
        }
    } catch (const cv::Exception &e) {
        qCritical() << "Error converting CV Mat to QImage:" << e.what();
        return QImage();
    }
}

/**
 * @brief Convert Qt QImage to OpenCV Mat
 * @param image QImage to convert
 * @return OpenCV Mat
 */
cv::Mat StencilGenerator::qImageToCvMat(const QImage &image) {
    try {
        if (image.isNull()) {
            return cv::Mat();
        }
        
        QImage converted = image;
        if (image.format() != QImage::Format_RGB888 && 
            image.format() != QImage::Format_Grayscale8) {
            converted = image.convertToFormat(QImage::Format_RGB888);
        }
        
        cv::Mat mat(converted.height(), converted.width(),
                   converted.format() == QImage::Format_Grayscale8 ? CV_8UC1 : CV_8UC3,
                   const_cast<uchar*>(converted.bits()),
                   static_cast<size_t>(converted.bytesPerLine()));
        
        cv::Mat result;
        mat.copyTo(result);  // Create a deep copy
        
        // Convert RGB to BGR if needed
        if (result.channels() == 3) {
            cv::cvtColor(result, result, cv::COLOR_RGB2BGR);
        }
        
        return result;
    } catch (...) {
        qCritical() << "Error converting QImage to CV Mat";
        return cv::Mat();
    }
}

// Additional helper implementations...

cv::Mat StencilGenerator::adjustBrightnessContrast(const cv::Mat &image, float alpha, float beta) {
    cv::Mat result;
    image.convertTo(result, -1, alpha, beta);
    return result;
}

cv::Mat StencilGenerator::detectFloatingIslands(const cv::Mat &binaryImage) {
    cv::Mat islands = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
    
    // Find connected components
    cv::Mat labels, stats, centroids;
    int numLabels = cv::connectedComponentsWithStats(binaryImage, labels, stats, centroids, 8);
    
    // Check which components touch the border
    cv::Mat borderMask = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
    borderMask.row(0).setTo(1);
    borderMask.row(borderMask.rows - 1).setTo(1);
    borderMask.col(0).setTo(1);
    borderMask.col(borderMask.cols - 1).setTo(1);
    
    for (int i = 1; i < numLabels; i++) {
        bool touchesBorder = false;
        
        // Check if component touches border
        for (int y = 0; y < labels.rows && !touchesBorder; y++) {
            const int* row = labels.ptr<int>(y);
            for (int x = 0; x < labels.cols; x++) {
                if (row[x] == i && borderMask.at<uchar>(y, x)) {
                    touchesBorder = true;
                    break;
                }
            }
        }
        
        // Mark as island if doesn't touch border
        if (!touchesBorder && stats.at<int>(i, cv::CC_STAT_AREA) > 50) {
            cv::Mat component = (labels == i);
            islands.setTo(255, component);
        }
    }
    
    return islands;
}

cv::Mat StencilGenerator::autoBridgeIslands(const cv::Mat &stencil, int bridgeWidth, uchar bridgeColor) {
    cv::Mat result = stencil.clone();
    cv::Mat islands = detectFloatingIslands(stencil);
    
    if (islands.empty()) {
        return result;
    }
    
    // Find contours of islands
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(islands, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    for (const auto& contour : contours) {
        if (contour.empty()) continue;
        
        // Find centroid
        cv::Moments m = cv::moments(contour);
        if (m.m00 == 0) continue;
        
        cv::Point centroid(static_cast<int>(m.m10 / m.m00),
                          static_cast<int>(m.m01 / m.m00));
        
        // Find closest border direction
        int leftDist = centroid.x;
        int rightDist = result.cols - centroid.x - 1;
        int topDist = centroid.y;
        int bottomDist = result.rows - centroid.y - 1;
        
        cv::Point endPoint;
        if (leftDist <= rightDist && leftDist <= topDist && leftDist <= bottomDist) {
            endPoint = cv::Point(0, centroid.y);
        } else if (rightDist <= leftDist && rightDist <= topDist && rightDist <= bottomDist) {
            endPoint = cv::Point(result.cols - 1, centroid.y);
        } else if (topDist <= leftDist && topDist <= rightDist && topDist <= bottomDist) {
            endPoint = cv::Point(centroid.x, 0);
        } else {
            endPoint = cv::Point(centroid.x, result.rows - 1);
        }
        
        // Draw bridge
        cv::line(result, centroid, endPoint, bridgeColor, bridgeWidth, cv::LINE_AA);
    }
    
    return result;
}