#pragma once
#include "VulkanWindow.h"
#include "VulkanEngine.h"
#include "RenderManager.h"
#include "Keyboard.h"
#include "Mouse.h"

class Game
{

private:
    VulkanWindow window;
    VulkanEngine vulkanEngine;
    RenderManager renderManager;
    LightManager light;
    std::shared_ptr<TextureManager> textureManager;

    std::shared_ptr<PerspectiveCamera> camera;
    Keyboard keyboard;
    Mouse mouse;

    std::vector<std::shared_ptr<Object2D>> SpriteList;
    std::vector<Model> ModelList;
    std::vector<Mesh> MeshList;
    SkyBoxMesh Skybox;

    void UpdateUniformBuffer(uint32_t currentImage);
    std::vector<Vertex> CalcVertex();

public:
    Game();
    ~Game();

    void MainLoop();
};
