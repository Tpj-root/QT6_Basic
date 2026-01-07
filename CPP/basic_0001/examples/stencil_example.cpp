// examples/stencil_example.cpp
#include "stencil_generator.hpp"
#include <iostream>

int main() {
    stencil::StencilGenerator generator;
    stencil::Preset preset;
    
    // Load an image
    if (!generator.loadImage("input.jpg")) {
        std::cerr << "Failed to load image" << std::endl;
        return 1;
    }
    
    // Generate stencil
    cv::Mat stencil = generator.generateStencil(preset);
    if (stencil.empty()) {
        std::cerr << "Failed to generate stencil" << std::endl;
        return 1;
    }
    
    // Detect floating islands
    auto islands = generator.detectFloatingIslands(stencil);
    if (!islands.empty()) {
        std::cout << "Found " << islands.size() << " floating islands" << std::endl;
        
        // Visualize islands
        cv::Mat visualization = generator.visualizeIslands(stencil, islands);
        
        // Auto-bridge islands
        cv::Mat bridged = generator.autoBridgeIslands(stencil, preset.bridge_width_px, 
                                                     preset.bridge_color);
        
        // Save results
        generator.saveStencilAsPNG("stencil.png", bridged);
    } else {
        generator.saveStencilAsPNG("stencil.png", stencil);
    }
    
    // Example of using sizing calculator
    auto size_info = stencil::SizingCalculator::calculatePumpkinSize(34.0f, 34.0f, "in");
    std::cout << "Usable area: " << size_info.usable_width << " x " 
              << size_info.usable_height << " " << size_info.units << std::endl;
    
    // Example of preset management
    stencil::PresetManager preset_manager;
    preset_manager.savePreset("HighContrast", preset);
    preset_manager.saveToFile("presets.json");
    
    return 0;
}