#include "InputManager.h"

std::shared_ptr<InputManager> InputManagerPtr::inputManager = nullptr;

InputManager::InputManager()
{
}

InputManager::InputManager(std::shared_ptr<VulkanWindow> window, std::shared_ptr<CameraManager> cameramanager)
{
	cameraManager = cameramanager;
	Window = window;
	keyboard = Keyboard();
	mouse = Mouse();
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
	keyboard.Update(Window, cameraManager->ActiveCamera);
	mouse.Update(Window, cameraManager->ActiveCamera);
}
