// stencil_generator.hpp
#ifndef STENCIL_GENERATOR_HPP
#define STENCIL_GENERATOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace stencil {

// ────────────────────────── PRESET STRUCTURE ──────────────────────────
struct Preset {
    std::string stencil_type = "Simple (2 colors)";
    int threshold = 128;
    int blur_amount = 2;
    bool edge_enhance = true;
    bool invert_colors = false;
    float contrast = 1.5f;
    int bridge_width_px = 8;
    int bridge_color = 255;
    std::string size_mode = "Round (pumpkin)";
    std::string units = "in";
    float horiz_circ = 34.0f;
    float vert_circ = 34.0f;
    float flat_w = 12.0f;
    float flat_h = 10.0f;
    
    // Serialization
    json to_json() const;
    static Preset from_json(const json& j);
};

// ────────────────────────── MAIN STENCIL GENERATOR ──────────────────────────
class StencilGenerator {
public:
    // Constructor/Destructor
    StencilGenerator();
    ~StencilGenerator();
    
    // Image loading
    bool loadImage(const std::string& filepath);
    bool loadImageFromMemory(const std::vector<uchar>& buffer);
    bool loadImageFromMat(const cv::Mat& image);
    
    // Stencil generation
    cv::Mat generateStencil(const Preset& preset);
    cv::Mat generateSimpleStencil(const Preset& preset);
    cv::Mat generateMultiLayerStencil(const Preset& preset);
    
    // Floating islands detection and bridging
    struct IslandInfo {
        std::vector<cv::Point> contour;
        cv::Rect bounding_box;
        double area;
        cv::Point centroid;
    };
    
    std::vector<IslandInfo> detectFloatingIslands(const cv::Mat& stencil);
    cv::Mat visualizeIslands(const cv::Mat& stencil, const std::vector<IslandInfo>& islands);
    cv::Mat autoBridgeIslands(const cv::Mat& stencil, int bridge_width = 6, int bridge_color = 255);
    
    // Drawing/touch-up
    cv::Mat applyDrawingMask(const cv::Mat& stencil, const cv::Mat& mask, 
                            bool paint_white = true, float scale_factor = 1.0f);
    
    // Utilities
    cv::Mat resizeForDisplay(const cv::Mat& image, int max_width = 560);
    cv::Mat convertToGrayscale(const cv::Mat& image);
    cv::Mat adjustContrast(const cv::Mat& image, float contrast);
    cv::Mat applyBlur(const cv::Mat& image, int blur_amount);
    cv::Mat enhanceEdges(const cv::Mat& image);
    cv::Mat applyThreshold(const cv::Mat& image, int threshold);
    cv::Mat invertImage(const cv::Mat& image);
    
    // File operations
    bool saveStencil(const std::string& filepath, const cv::Mat& stencil);
    bool saveStencilAsPNG(const std::string& filepath, const cv::Mat& stencil);
    bool saveStencilAsBMP(const std::string& filepath, const cv::Mat& stencil);
    
    // Getter methods
    cv::Mat getOriginalImage() const { return original_image_; }
    cv::Mat getProcessedImage() const { return processed_image_; }
    cv::Mat getCurrentStencil() const { return current_stencil_; }
    bool hasImage() const { return !original_image_.empty(); }
    
private:
    // Image storage
    cv::Mat original_image_;
    cv::Mat processed_image_;
    cv::Mat current_stencil_;
    
    // Helper methods
    cv::Mat preprocessImage(const cv::Mat& image, const Preset& preset);
    cv::Mat createContourMask(const cv::Mat& binary_image);
};

// ────────────────────────── PRESET MANAGER ──────────────────────────
class PresetManager {
public:
    PresetManager();
    ~PresetManager();
    
    // Preset management
    bool savePreset(const std::string& name, const Preset& preset);
    bool loadPreset(const std::string& name, Preset& preset);
    bool deletePreset(const std::string& name);
    
    // List operations
    std::vector<std::string> getPresetNames() const;
    bool hasPreset(const std::string& name) const;
    
    // File operations
    bool saveToFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath);
    bool exportToJSON(const std::string& filepath);
    bool importFromJSON(const std::string& filepath);
    
private:
    std::unordered_map<std::string, Preset> presets_;
};

// ────────────────────────── SIZING CALCULATOR ──────────────────────────
class SizingCalculator {
public:
    struct SizeInfo {
        float width;
        float height;
        float usable_width;
        float usable_height;
        std::string units;
    };
    
    static SizeInfo calculatePumpkinSize(float horiz_circ, float vert_circ, 
                                        const std::string& units = "in");
    static SizeInfo calculateFlatSize(float width, float height, 
                                     const std::string& units = "in");
    static cv::Mat generateReferenceDiagram(const std::string& mode, 
                                           const SizeInfo& size_info);
    
private:
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float USABLE_RATIO = 0.8f; // 80% of diameter
};

} // namespace stencil

#endif // STENCIL_GENERATOR_HPP