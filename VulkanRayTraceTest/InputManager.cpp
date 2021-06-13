#include "InputManager.h"

InputManager::InputManager()
{
}

InputManager::InputManager(std::shared_ptr<VulkanWindow> window)
{
	Window = window;
	keyboard = Keyboard();
	mouse = Mouse();
}

InputManager::~InputManager()
{
}

void InputManager::Update(CameraManager& cameraManager)
{
	keyboard.Update(Window, cameraManager.ActiveCamera);
	mouse.Update(Window, cameraManager.ActiveCamera);
}
