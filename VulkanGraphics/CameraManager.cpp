#include "CameraManager.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"

std::shared_ptr<CameraManager> CameraManagerPtr::cameraManager = nullptr;

CameraManager::CameraManager()
{
}

CameraManager::CameraManager(std::shared_ptr<VulkanEngine> Engine)
{
    engine = Engine;

    CameraList.emplace_back(std::make_shared<PerspectiveCamera>(PerspectiveCamera(glm::vec2(engine->SwapChain.SwapChainResolution.width, engine->SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f))));
    ActiveCamera = CameraList.front();

    CameraList.emplace_back(std::make_shared<OrthographicCamera>(OrthographicCamera(glm::vec2(engine->SwapChain.SwapChainResolution.width, engine->SwapChain.SwapChainResolution.height), glm::vec3(3.0f, 8.0f, 5.0f), 3.1f)));
    ActiveCamera = CameraList.front();
}

CameraManager::~CameraManager()
{
}

void CameraManager::Update()
{
    ActiveCamera = CameraList[cameraIndex];
    ActiveCamera->Update(engine);
}

void CameraManager::ClearCameras()
{
    ActiveCamera = nullptr;
    CameraList.clear();
}
