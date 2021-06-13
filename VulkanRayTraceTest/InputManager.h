#pragma once
#include "CameraManager.h"
#include "Keyboard.h"
#include "Mouse.h"

class InputManager
{
private:
	std::shared_ptr<VulkanWindow> Window;

public:
	Keyboard keyboard;
	Mouse mouse;

	InputManager();
	InputManager(std::shared_ptr<VulkanWindow> window);
	~InputManager();

	void Update(CameraManager& cameraManager);
	bool IsKeyPressed(KeyboardKey key) { return keyboard.IsKeyPressed(Window, key); }
	bool IsKeyReleased(KeyboardKey key) { return keyboard.IsKeyReleased(Window, key); }
};

