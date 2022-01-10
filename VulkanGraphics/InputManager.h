#pragma once
#include "CameraManager.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameController.h"

class InputManager
{
private:

public:
	GameController controller;
	Keyboard keyboard;
	Mouse mouse;

	InputManager();
	InputManager(std::shared_ptr<VulkanWindow> window);
	~InputManager();

	void Update();
	bool IsKeyPressed(int key) { return keyboard.IsKeyPressed(key); }
	bool IsKeyReleased(int key) { return keyboard.IsKeyReleased(key); }
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
