#pragma once
#include "../VulkanGraphics/VulkanWindow.h"

class Game
{

private:
    VulkanWindow window;
   // VulkanEngine vulkanEngine;
    //RenderManager renderManager;
    //LightManager light;
    //std::shared_ptr<TextureManager> textureManager;

    //std::shared_ptr<PerspectiveCamera> camera;
    //Keyboard keyboard;
    //Mouse mouse;

    //std::vector<Model> ModelList;
    //SkyBoxMesh Skybox;

    void UpdateUniformBuffer(uint32_t currentImage);
public:
    Game();
    ~Game();

    void MainLoop();
};
