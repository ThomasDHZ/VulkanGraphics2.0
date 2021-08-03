#include "CameraManager.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"

CameraManager::CameraManager()
{
}

CameraManager::CameraManager(std::shared_ptr<VulkanEngine> engine)
{
    CameraList.emplace_back(std::make_shared<PerspectiveCamera>(PerspectiveCamera(glm::vec2(engine->SwapChain.SwapChainResolution.width, engine->SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f))));
    ActiveCamera = CameraList.front();

    CameraList.emplace_back(std::make_shared<OrthographicCamera>(OrthographicCamera(glm::vec2(engine->SwapChain.SwapChainResolution.width, engine->SwapChain.SwapChainResolution.height), glm::vec3(3.0f, 8.0f, 5.0f), 3.1f)));
    ActiveCamera = CameraList.front();
}

CameraManager::~CameraManager()
{
}

void CameraManager::Update(std::shared_ptr<VulkanEngine> engine)
{
    ActiveCamera = CameraList[cameraIndex];
    ActiveCamera->Update(engine);
}
