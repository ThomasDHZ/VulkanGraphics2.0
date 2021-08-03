#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include "VulkanWindow.h"
#include "Vulkanengine.h"
#include "Renderer.h"
#include <map>
#include "FontTexture.h"
#include "Font.h"

class Engine {
private:
    std::shared_ptr<VulkanEngine> engine;
    Renderer renderer;
    std::shared_ptr<VulkanWindow> window;
    std::shared_ptr<AssetManager> assetManager;
    std::shared_ptr<Font> font;

public:
    Engine();
    Engine(unsigned int width, unsigned int height, const char* WindowName);
    ~Engine();

    void MainLoop();
    std::shared_ptr<Texture> LoadTexture2D(const std::string& FilePath, VkFormat format);
    std::shared_ptr<Material> LoadMaterial(const std::string& MaterialName, MaterialTexture& material);
    std::shared_ptr<Material> LoadMaterial(const std::string& MaterialName, std::shared_ptr<Material> material);
    void LoadCubeMap(std::string CubeMapFiles[6]);
    void LoadTerrain(const std::string& HeightMapPath, std::shared_ptr<Material> material);
};