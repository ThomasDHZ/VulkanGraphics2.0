#pragma once
#include "Vulkanengine.h"
#include "Camera.h"

class CameraManager
{
private:
	std::shared_ptr<VulkanEngine> engine;
public:
	std::shared_ptr<Camera> ActiveCamera;
	std::vector<std::shared_ptr<Camera>> CameraList;
	int cameraIndex = 0;

	CameraManager();
	CameraManager(std::shared_ptr<VulkanEngine> engine);
	~CameraManager();

	void Update();
	void ClearCameras();
};

class CameraManagerPtr
{
private:
	static std::shared_ptr<CameraManager> cameraManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		if (cameraManager == nullptr)
		{
			cameraManager = std::make_shared<CameraManager>(CameraManager(engine));
		}
		else
		{
			std::cout << "Camera Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<CameraManager> GetCameraManagerPtr()
	{
		return cameraManager;
	}
};
