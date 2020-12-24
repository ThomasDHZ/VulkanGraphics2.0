#pragma once
#include "VulkanWindow.h"
#include "VulkanEngine.h"
#include "RenderManager.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <Object2D.h>
#include "OrthographicCamera.h"
class Game
{

private:
    VulkanWindow window;
    VulkanEngine vulkanEngine;
    RenderManager renderManager;
    LightManager light;
    std::shared_ptr<TextureManager> textureManager;

    std::shared_ptr<PerspectiveCamera> camera;
    std::shared_ptr<OrthographicCamera> orthocamera;
    Keyboard keyboard;
    Mouse mouse;

    std::vector<std::shared_ptr<Object2D>> SpriteList;
    std::vector<Model> ModelList;
    SkyBoxMesh Skybox;

    void UpdateUniformBuffer(uint32_t currentImage);
public:
    Game();
    ~Game();

    void MainLoop();
};