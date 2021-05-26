#pragma once
#include "VulkanEngine.h"
#include "Camera.h"

class CameraManager
{
private:
public:
	std::shared_ptr<Camera> ActiveCamera;
	std::vector<std::shared_ptr<Camera>> CameraList;
	int cameraIndex = 0;

	CameraManager();
	CameraManager(VulkanEngine& engine);
	~CameraManager();

	void Update(VulkanEngine& engine);
};

