#pragma once
#include "CameraManager.h"
#include "Keyboard.h"
#include "Mouse.h"

class InputManager
{
private:
public:
	Keyboard keyboard;
	Mouse mouse;

	InputManager();
	~InputManager();

	void Update(VulkanWindow& window, CameraManager& cameraManager);
};

