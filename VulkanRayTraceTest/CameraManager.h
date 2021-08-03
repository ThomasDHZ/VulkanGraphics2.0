#pragma once
#include "Vulkanengine.h"
#include "Camera.h"

class CameraManager
{
private:
public:
	std::shared_ptr<Camera> ActiveCamera;
	std::vector<std::shared_ptr<Camera>> CameraList;
	int cameraIndex = 0;

	CameraManager();
	CameraManager(std::shared_ptr<VulkanEngine> engine);
	~CameraManager();

	void Update(std::shared_ptr<VulkanEngine> engine);
};

