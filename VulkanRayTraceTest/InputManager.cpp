#include "InputManager.h"

InputManager::InputManager()
{
	keyboard = Keyboard();
	mouse = Mouse();
}

InputManager::~InputManager()
{
}

void InputManager::Update(VulkanWindow& window, CameraManager& cameraManager)
{
	keyboard.Update(window.GetWindowPtr(), cameraManager.ActiveCamera);
	mouse.Update(window.GetWindowPtr(), cameraManager.ActiveCamera);
}
