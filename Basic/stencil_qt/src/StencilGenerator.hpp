#ifndef STENCILGENERATOR_HPP
#define STENCILGENERATOR_HPP

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QObject>
#include <vector>
#include <string>

/**
 * @brief Processing mode for stencil generation
 */
enum class ProcessingMode {
    SIMPLE_THRESHOLD,    // Simple black/white threshold
    EDGE_DETECTION,      // Edge detection (wireframe)
    ADAPTIVE_THRESHOLD,  // Adaptive thresholding
    MULTI_LAYER,         // Multi-layer stencil
    CONTOUR_POLYGON,     // Polygon simplification
    DETAIL_PRESERVING    // Detail preserving threshold
};

/**
 * @brief Parameters for stencil generation
 */
struct StencilParams {
    ProcessingMode mode = ProcessingMode::SIMPLE_THRESHOLD;
    
    // Common parameters
    int threshold = 128;
    float contrast = 1.5f;
    float brightness = 0.0f;
    int blurRadius = 2;
    
    // Edge detection parameters
    int edgeLowThreshold = 50;
    int edgeHighThreshold = 150;
    int edgeKernelSize = 3;
    
    // Adaptive threshold parameters
    int adaptiveBlockSize = 11;
    int adaptiveC = 2;
    
    // Polygon simplification parameters
    double polygonEpsilon = 2.0;
    int minContourArea = 100;
    
    // Multi-layer parameters
    int layerCount = 4;
    bool invertColors = false;
    bool preserveEdges = true;
    
    // Output options
    int outputWidth = 0;  // 0 = keep original
    int outputHeight = 0;
    bool maintainAspectRatio = true;
};

/**
 * @brief Stencil processing result
 */
struct StencilResult {
    QImage previewImage;
    QImage fullResolutionImage;
    cv::Mat processedMat;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    QString errorMessage;
    bool success = false;
    
    // Statistics
    int blackPixels = 0;
    int whitePixels = 0;
    int islandCount = 0;
    double processingTimeMs = 0.0;
};

/**
 * @brief Main stencil generator class
 * Handles image processing, live preview, and stencil generation
 */
class StencilGenerator : public QObject {
    Q_OBJECT
    
public:
    explicit StencilGenerator(QObject *parent = nullptr);
    ~StencilGenerator();
    
    // Image loading
    bool loadImage(const QString &filePath);
    bool loadImage(const QImage &image);
    bool loadImage(const cv::Mat &image);
    
    // Processing functions
    StencilResult generateStencil(const StencilParams &params);
    StencilResult generateLivePreview(const StencilParams &params, int maxPreviewSize = 800);
    
    // Processing modes
    cv::Mat applySimpleThreshold(const cv::Mat &image, const StencilParams &params);
    cv::Mat applyEdgeDetection(const cv::Mat &image, const StencilParams &params);
    cv::Mat applyAdaptiveThreshold(const cv::Mat &image, const StencilParams &params);
    cv::Mat applyMultiLayer(const cv::Mat &image, const StencilParams &params);
    cv::Mat applyContourPolygon(const cv::Mat &image, const StencilParams &params);
    cv::Mat applyDetailPreserving(const cv::Mat &image, const StencilParams &params);
    
    // Utility functions
    cv::Mat preprocessImage(const cv::Mat &image, const StencilParams &params);
    cv::Mat adjustBrightnessContrast(const cv::Mat &image, float alpha, float beta);
    cv::Mat detectFloatingIslands(const cv::Mat &binaryImage);
    cv::Mat autoBridgeIslands(const cv::Mat &stencil, int bridgeWidth = 3, uchar bridgeColor = 255);
    
    // Conversion functions
    static QImage cvMatToQImage(const cv::Mat &mat);
    static cv::Mat qImageToCvMat(const QImage &image);
    
    // Getters
    cv::Mat getOriginalImage() const { return originalImage_; }
    QImage getOriginalQImage() const { return cvMatToQImage(originalImage_); }
    bool hasImage() const { return !originalImage_.empty(); }
    
    // Image statistics
    struct ImageStats {
        int width = 0;
        int height = 0;
        int channels = 0;
        double minVal = 0.0;
        double maxVal = 0.0;
        double meanVal = 0.0;
        double stdDev = 0.0;
    };
    
    ImageStats getImageStats() const;
    
signals:
    void processingStarted();
    void processingProgress(int percent);
    void processingCompleted(const StencilResult &result);
    void processingError(const QString &error);
    
private:
    cv::Mat originalImage_;
    cv::Mat processedImage_;
    
    // Helper functions
    cv::Mat resizeImage(const cv::Mat &image, int maxSize, bool keepAspect = true);
    cv::Mat convertToGrayscale(const cv::Mat &image);
    cv::Mat applyGaussianBlur(const cv::Mat &image, int radius);
    cv::Mat applyMedianBlur(const cv::Mat &image, int radius);
    cv::Mat applyBilateralFilter(const cv::Mat &image, int d, double sigmaColor, double sigmaSpace);
    
    // Edge enhancement
    cv::Mat enhanceEdges(const cv::Mat &image, float strength = 1.0f);
    
    // Contour processing
    std::vector<std::vector<cv::Point>> findContours(const cv::Mat &binaryImage);
    cv::Mat drawContours(const cv::Mat &image, const std::vector<std::vector<cv::Point>> &contours,
                         const cv::Scalar &color = cv::Scalar(0, 255, 0), int thickness = 1);
    
    // Polygon simplification
    std::vector<cv::Point> simplifyPolygon(const std::vector<cv::Point> &contour, double epsilon);
    
    // Island detection
    std::vector<std::vector<cv::Point>> detectIslands(const cv::Mat &binaryImage);
    
    // Noise reduction
    cv::Mat removeSmallNoise(const cv::Mat &binaryImage, int minArea = 25);
    cv::Mat fillSmallHoles(const cv::Mat &binaryImage, int maxArea = 100);
};

#endif // STENCILGENERATOR_HPP