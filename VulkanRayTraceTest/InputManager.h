#pragma once
#include "CameraManager.h"
#include "Keyboard.h"
#include "Mouse.h"

class InputManager
{
private:
	std::shared_ptr<VulkanWindow> Window;
	std::shared_ptr<CameraManager> cameraManager;
public:
	Keyboard keyboard;
	Mouse mouse;

	InputManager();
	InputManager(std::shared_ptr<VulkanWindow> window, std::shared_ptr<CameraManager> cameraManager);
	~InputManager();

	void Update();
	bool IsKeyPressed(KeyboardKey key) { return keyboard.IsKeyPressed(Window, key); }
	bool IsKeyReleased(KeyboardKey key) { return keyboard.IsKeyReleased(Window, key); }
};

