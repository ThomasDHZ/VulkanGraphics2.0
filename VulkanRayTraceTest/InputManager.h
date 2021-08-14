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
	InputManager(std::shared_ptr<VulkanWindow> window);
	~InputManager();

	void Update();
	bool IsKeyPressed(KeyboardKey key) { return keyboard.IsKeyPressed(WindowPtr::GetWindowPtr(), key); }
	bool IsKeyReleased(KeyboardKey key) { return keyboard.IsKeyReleased(WindowPtr::GetWindowPtr(), key); }
};

class InputManagerPtr
{
private:
	static std::shared_ptr<InputManager> inputManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanWindow> window, std::shared_ptr<CameraManager> camera)
	{
		if (inputManager == nullptr)
		{
			inputManager = std::make_shared<InputManager>(InputManager(window));
		}
		else
		{
			std::cout << "Input Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<InputManager> GetInputManagerPtr()
	{
		return inputManager;
	}
};
