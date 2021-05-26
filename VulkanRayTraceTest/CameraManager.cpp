#include "CameraManager.h"
#include "PerspectiveCamera.h"

CameraManager::CameraManager()
{
}

CameraManager::CameraManager(VulkanEngine& engine)
{
    CameraList.emplace_back(std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f)));
    ActiveCamera = CameraList.front();

    CameraList.emplace_back(std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f)));
    ActiveCamera = CameraList.front();
}

CameraManager::~CameraManager()
{
}

void CameraManager::Update(VulkanEngine& engine)
{
    ActiveCamera = CameraList[cameraIndex];
    ActiveCamera->Update(engine);
}
