// stencil_generator.cpp
#include "stencil_generator.hpp"
#include <fstream>
#include <cmath>
#include <algorithm>

namespace stencil {

// ────────────────────────── PRESET IMPLEMENTATION ──────────────────────────
json Preset::to_json() const {
    return {
        {"stencil_type", stencil_type},
        {"threshold", threshold},
        {"blur_amount", blur_amount},
        {"edge_enhance", edge_enhance},
        {"invert_colors", invert_colors},
        {"contrast", contrast},
        {"bridge_width_px", bridge_width_px},
        {"bridge_color", bridge_color},
        {"size_mode", size_mode},
        {"units", units},
        {"horiz_circ", horiz_circ},
        {"vert_circ", vert_circ},
        {"flat_w", flat_w},
        {"flat_h", flat_h}
    };
}

Preset Preset::from_json(const json& j) {
    Preset preset;
    if (j.contains("stencil_type")) preset.stencil_type = j["stencil_type"];
    if (j.contains("threshold")) preset.threshold = j["threshold"];
    if (j.contains("blur_amount")) preset.blur_amount = j["blur_amount"];
    if (j.contains("edge_enhance")) preset.edge_enhance = j["edge_enhance"];
    if (j.contains("invert_colors")) preset.invert_colors = j["invert_colors"];
    if (j.contains("contrast")) preset.contrast = j["contrast"];
    if (j.contains("bridge_width_px")) preset.bridge_width_px = j["bridge_width_px"];
    if (j.contains("bridge_color")) preset.bridge_color = j["bridge_color"];
    if (j.contains("size_mode")) preset.size_mode = j["size_mode"];
    if (j.contains("units")) preset.units = j["units"];
    if (j.contains("horiz_circ")) preset.horiz_circ = j["horiz_circ"];
    if (j.contains("vert_circ")) preset.vert_circ = j["vert_circ"];
    if (j.contains("flat_w")) preset.flat_w = j["flat_w"];
    if (j.contains("flat_h")) preset.flat_h = j["flat_h"];
    return preset;
}

// ────────────────────────── STENCIL GENERATOR IMPLEMENTATION ──────────────────────────
StencilGenerator::StencilGenerator() {
    // Initialize OpenCV if needed
}

StencilGenerator::~StencilGenerator() {
    // Cleanup if needed
}

bool StencilGenerator::loadImage(const std::string& filepath) {
    original_image_ = cv::imread(filepath, cv::IMREAD_COLOR);
    if (original_image_.empty()) {
        return false;
    }
    return true;
}

bool StencilGenerator::loadImageFromMemory(const std::vector<uchar>& buffer) {
    original_image_ = cv::imdecode(buffer, cv::IMREAD_COLOR);
    return !original_image_.empty();
}

bool StencilGenerator::loadImageFromMat(const cv::Mat& image) {
    if (image.empty()) return false;
    image.copyTo(original_image_);
    return true;
}

cv::Mat StencilGenerator::convertToGrayscale(const cv::Mat& image) {
    if (image.channels() == 3) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        return gray;
    }
    return image.clone();
}

cv::Mat StencilGenerator::adjustContrast(const cv::Mat& image, float contrast) {
    cv::Mat result;
    if (contrast == 1.0f) {
        return image.clone();
    }
    
    // Convert to float for contrast adjustment
    cv::Mat float_img;
    image.convertTo(float_img, CV_32F);
    
    // Apply contrast
    float_img = (float_img - 127.0f) * contrast + 127.0f;
    
    // Clip values to [0, 255]
    cv::threshold(float_img, float_img, 255, 255, cv::THRESH_TRUNC);
    cv::threshold(float_img, float_img, 0, 0, cv::THRESH_TOZERO);
    
    float_img.convertTo(result, image.type());
    return result;
}

cv::Mat StencilGenerator::applyBlur(const cv::Mat& image, int blur_amount) {
    if (blur_amount <= 0) {
        return image.clone();
    }
    
    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(0, 0), blur_amount);
    return blurred;
}

cv::Mat StencilGenerator::enhanceEdges(const cv::Mat& image) {
    cv::Mat edges, sharpened;
    
    // Apply edge enhancement using Laplacian
    cv::Laplacian(image, edges, CV_16S, 3);
    cv::convertScaleAbs(edges, edges);
    
    // Add edges to original for enhancement
    cv::addWeighted(image, 1.5, edges, -0.5, 0, sharpened);
    return sharpened;
}

cv::Mat StencilGenerator::applyThreshold(const cv::Mat& image, int threshold) {
    cv::Mat binary;
    cv::threshold(image, binary, threshold, 255, cv::THRESH_BINARY);
    return binary;
}

cv::Mat StencilGenerator::invertImage(const cv::Mat& image) {
    cv::Mat inverted;
    cv::bitwise_not(image, inverted);
    return inverted;
}

cv::Mat StencilGenerator::resizeForDisplay(const cv::Mat& image, int max_width) {
    if (image.cols <= max_width) {
        return image.clone();
    }
    
    float scale = static_cast<float>(max_width) / image.cols;
    cv::Size new_size(max_width, static_cast<int>(image.rows * scale));
    
    cv::Mat resized;
    cv::resize(image, resized, new_size, 0, 0, cv::INTER_LANCZOS4);
    return resized;
}

cv::Mat StencilGenerator::preprocessImage(const cv::Mat& image, const Preset& preset) {
    cv::Mat processed = convertToGrayscale(image);
    processed = adjustContrast(processed, preset.contrast);
    return processed;
}

cv::Mat StencilGenerator::generateSimpleStencil(const Preset& preset) {
    if (original_image_.empty()) {
        return cv::Mat();
    }
    
    cv::Mat processed = preprocessImage(original_image_, preset);
    
    // Apply blur if needed
    if (preset.blur_amount > 0) {
        processed = applyBlur(processed, preset.blur_amount);
    }
    
    // Apply edge enhancement if needed
    if (preset.edge_enhance) {
        processed = enhanceEdges(processed);
    }
    
    // Apply threshold
    cv::Mat stencil = applyThreshold(processed, preset.threshold);
    
    // Invert if needed
    if (preset.invert_colors) {
        stencil = invertImage(stencil);
    }
    
    return stencil;
}

cv::Mat StencilGenerator::generateMultiLayerStencil(const Preset& preset) {
    if (original_image_.empty()) {
        return cv::Mat();
    }
    
    cv::Mat processed = preprocessImage(original_image_, preset);
    
    // Apply blur if needed
    if (preset.blur_amount > 0) {
        processed = applyBlur(processed, preset.blur_amount);
    }
    
    // Create multiple layers
    cv::Mat layer1, layer2, layer3, layer4;
    cv::threshold(processed, layer1, 192, 255, cv::THRESH_BINARY);
    cv::threshold(processed, layer2, 128, 170, cv::THRESH_BINARY);
    cv::threshold(processed, layer3, 64, 85, cv::THRESH_BINARY);
    cv::threshold(processed, layer4, 64, 0, cv::THRESH_BINARY_INV);
    
    // Combine layers
    cv::Mat stencil = cv::Mat::zeros(processed.size(), CV_8UC1);
    stencil = layer1 + layer2 + layer3 + layer4;
    
    // Invert if needed
    if (preset.invert_colors) {
        stencil = invertImage(stencil);
    }
    
    return stencil;
}

cv::Mat StencilGenerator::generateStencil(const Preset& preset) {
    cv::Mat stencil;
    
    if (preset.stencil_type.find("Simple") != std::string::npos) {
        stencil = generateSimpleStencil(preset);
    } else {
        stencil = generateMultiLayerStencil(preset);
    }
    
    if (!stencil.empty()) {
        current_stencil_ = stencil.clone();
    }
    
    return stencil;
}

std::vector<StencilGenerator::IslandInfo> StencilGenerator::detectFloatingIslands(const cv::Mat& stencil) {
    std::vector<IslandInfo> islands;
    
    if (stencil.empty()) {
        return islands;
    }
    
    // Create binary mask (black areas = 1, white areas = 0)
    cv::Mat binary;
    cv::threshold(stencil, binary, 128, 1, cv::THRESH_BINARY_INV);
    
    // Find connected components
    cv::Mat labels, stats, centroids;
    int num_components = cv::connectedComponentsWithStats(binary, labels, stats, centroids, 8);
    
    // Create border mask
    cv::Mat border_mask = cv::Mat::zeros(binary.size(), CV_8UC1);
    border_mask.row(0).setTo(1);
    border_mask.row(border_mask.rows - 1).setTo(1);
    border_mask.col(0).setTo(1);
    border_mask.col(border_mask.cols - 1).setTo(1);
    
    // Collect border component IDs
    std::vector<int> border_ids;
    for (int y = 0; y < labels.rows; y++) {
        for (int x = 0; x < labels.cols; x++) {
            if (border_mask.at<uchar>(y, x)) {
                int label = labels.at<int>(y, x);
                if (std::find(border_ids.begin(), border_ids.end(), label) == border_ids.end()) {
                    border_ids.push_back(label);
                }
            }
        }
    }
    
    // Find islands (components not touching border)
    for (int i = 1; i < num_components; i++) {
        // Skip components touching border
        if (std::find(border_ids.begin(), border_ids.end(), i) != border_ids.end()) {
            continue;
        }
        
        double area = stats.at<int>(i, cv::CC_STAT_AREA);
        
        // Filter small islands
        if (area > 50) {
            IslandInfo info;
            info.area = area;
            info.bounding_box = cv::Rect(
                stats.at<int>(i, cv::CC_STAT_LEFT),
                stats.at<int>(i, cv::CC_STAT_TOP),
                stats.at<int>(i, cv::CC_STAT_WIDTH),
                stats.at<int>(i, cv::CC_STAT_HEIGHT)
            );
            info.centroid = cv::Point(
                static_cast<int>(centroids.at<double>(i, 0)),
                static_cast<int>(centroids.at<double>(i, 1))
            );
            
            // Extract contour
            cv::Mat component_mask = (labels == i);
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(component_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            if (!contours.empty()) {
                info.contour = contours[0];
                islands.push_back(info);
            }
        }
    }
    
    return islands;
}

cv::Mat StencilGenerator::visualizeIslands(const cv::Mat& stencil, const std::vector<IslandInfo>& islands) {
    cv::Mat visualization;
    cv::cvtColor(stencil, visualization, cv::COLOR_GRAY2BGR);
    
    for (const auto& island : islands) {
        std::vector<std::vector<cv::Point>> contours = {island.contour};
        cv::drawContours(visualization, contours, 0, cv::Scalar(0, 0, 255), 2);
    }
    
    return visualization;
}

cv::Mat StencilGenerator::autoBridgeIslands(const cv::Mat& stencil, int bridge_width, int bridge_color) {
    if (stencil.empty()) {
        return cv::Mat();
    }
    
    cv::Mat result = stencil.clone();
    std::vector<IslandInfo> islands = detectFloatingIslands(stencil);
    
    for (const auto& island : islands) {
        cv::Point center = island.centroid;
        int h = stencil.rows;
        int w = stencil.cols;
        
        // Find closest direction to border
        std::map<std::string, int> distances = {
            {"up", center.y},
            {"down", h - 1 - center.y},
            {"left", center.x},
            {"right", w - 1 - center.x}
        };
        
        auto min_dir = std::min_element(distances.begin(), distances.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        
        cv::Point end_point;
        if (min_dir->first == "up") {
            end_point = cv::Point(center.x, 0);
        } else if (min_dir->first == "down") {
            end_point = cv::Point(center.x, h - 1);
        } else if (min_dir->first == "left") {
            end_point = cv::Point(0, center.y);
        } else { // right
            end_point = cv::Point(w - 1, center.y);
        }
        
        // Draw bridge line
        cv::line(result, center, end_point, bridge_color, 
                std::max(1, bridge_width), cv::LINE_AA);
    }
    
    return result;
}

cv::Mat StencilGenerator::applyDrawingMask(const cv::Mat& stencil, const cv::Mat& mask, 
                                          bool paint_white, float scale_factor) {
    if (stencil.empty() || mask.empty()) {
        return cv::Mat();
    }
    
    cv::Mat result = stencil.clone();
    cv::Mat resized_mask;
    
    // Resize mask if needed
    if (std::abs(scale_factor - 1.0f) > 0.01f) {
        cv::Size new_size(
            static_cast<int>(mask.cols * scale_factor),
            static_cast<int>(mask.rows * scale_factor)
        );
        cv::resize(mask, resized_mask, new_size, 0, 0, cv::INTER_NEAREST);
    } else {
        resized_mask = mask;
    }
    
    // Ensure mask matches stencil size
    if (resized_mask.size() != stencil.size()) {
        cv::resize(resized_mask, resized_mask, stencil.size(), 0, 0, cv::INTER_NEAREST);
    }
    
    // Convert mask to binary
    cv::Mat binary_mask;
    cv::threshold(resized_mask, binary_mask, 1, 1, cv::THRESH_BINARY);
    
    // Apply mask
    if (paint_white) {
        result.setTo(255, binary_mask);
    } else {
        result.setTo(0, binary_mask);
    }
    
    return result;
}

bool StencilGenerator::saveStencil(const std::string& filepath, const cv::Mat& stencil) {
    if (stencil.empty()) {
        return false;
    }
    
    // Convert to binary (black and white) for saving
    cv::Mat binary;
    cv::threshold(stencil, binary, 127, 255, cv::THRESH_BINARY);
    
    return cv::imwrite(filepath, binary);
}

bool StencilGenerator::saveStencilAsPNG(const std::string& filepath, const cv::Mat& stencil) {
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9); // Maximum compression
    
    cv::Mat binary;
    cv::threshold(stencil, binary, 127, 255, cv::THRESH_BINARY);
    
    return cv::imwrite(filepath, binary, compression_params);
}

bool StencilGenerator::saveStencilAsBMP(const std::string& filepath, const cv::Mat& stencil) {
    cv::Mat binary;
    cv::threshold(stencil, binary, 127, 255, cv::THRESH_BINARY);
    return cv::imwrite(filepath, binary);
}

// ────────────────────────── PRESET MANAGER IMPLEMENTATION ──────────────────────────
PresetManager::PresetManager() {
    // Add default preset
    presets_["Default"] = Preset();
}

PresetManager::~PresetManager() {}

bool PresetManager::savePreset(const std::string& name, const Preset& preset) {
    if (name.empty()) {
        return false;
    }
    presets_[name] = preset;
    return true;
}

bool PresetManager::loadPreset(const std::string& name, Preset& preset) {
    auto it = presets_.find(name);
    if (it != presets_.end()) {
        preset = it->second;
        return true;
    }
    return false;
}

bool PresetManager::deletePreset(const std::string& name) {
    if (name == "Default") {
        return false; // Don't delete default
    }
    return presets_.erase(name) > 0;
}

std::vector<std::string> PresetManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& pair : presets_) {
        names.push_back(pair.first);
    }
    return names;
}

bool PresetManager::hasPreset(const std::string& name) const {
    return presets_.find(name) != presets_.end();
}

bool PresetManager::saveToFile(const std::string& filepath) {
    try {
        json j;
        for (const auto& pair : presets_) {
            j[pair.first] = pair.second.to_json();
        }
        
        std::ofstream file(filepath);
        file << j.dump(2);
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool PresetManager::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        file.close();
        
        for (auto& element : j.items()) {
            presets_[element.key()] = Preset::from_json(element.value());
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool PresetManager::exportToJSON(const std::string& filepath) {
    return saveToFile(filepath);
}

bool PresetManager::importFromJSON(const std::string& filepath) {
    return loadFromFile(filepath);
}

// ────────────────────────── SIZING CALCULATOR IMPLEMENTATION ──────────────────────────
SizingCalculator::SizeInfo SizingCalculator::calculatePumpkinSize(float horiz_circ, float vert_circ, 
                                                                 const std::string& units) {
    SizeInfo info;
    info.units = units;
    
    // Calculate diameters from circumferences
    float horiz_diam = horiz_circ / PI;
    float vert_diam = vert_circ / PI;
    
    info.width = horiz_diam;
    info.height = vert_diam;
    info.usable_width = horiz_diam * USABLE_RATIO;
    info.usable_height = vert_diam * USABLE_RATIO;
    
    return info;
}

SizingCalculator::SizeInfo SizingCalculator::calculateFlatSize(float width, float height, 
                                                              const std::string& units) {
    SizeInfo info;
    info.units = units;
    info.width = width;
    info.height = height;
    info.usable_width = width;
    info.usable_height = height;
    return info;
}

cv::Mat SizingCalculator::generateReferenceDiagram(const std::string& mode, 
                                                  const SizeInfo& size_info) {
    cv::Mat diagram(400, 600, CV_8UC3, cv::Scalar(255, 255, 255));
    
    if (mode.find("Round") != std::string::npos) {
        // Draw pumpkin reference
        cv::Point center(diagram.cols / 2, diagram.rows / 2 + 30);
        int rx = static_cast<int>(diagram.cols * 0.24);
        int ry = static_cast<int>(diagram.rows * 0.18);
        
        // Draw outer ellipse
        cv::ellipse(diagram, center, cv::Size(rx, ry), 0, 0, 360, cv::Scalar(0, 0, 0), 2);
        
        // Draw inner ellipse (slightly smaller)
        cv::ellipse(diagram, center, cv::Size(static_cast<int>(rx * 0.9), 
                                             static_cast<int>(ry * 0.9)), 
                   0, 0, 360, cv::Scalar(0, 0, 0), 2);
        
        // Draw stem
        cv::rectangle(diagram, 
                     cv::Rect(center.x - 30, center.y - ry - 60, 60, 30),
                     cv::Scalar(0, 0, 0), 2);
        
    } else {
        // Draw flat surface reference
        cv::Point center(diagram.cols / 2, diagram.rows / 2);
        int width = 200;
        int height = 120;
        
        cv::rectangle(diagram, 
                     cv::Rect(center.x - width/2, center.y - height/2, width, height),
                     cv::Scalar(0, 0, 0), 2);
    }
    
    return diagram;
}

} // namespace stencil