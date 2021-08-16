#include "InputManager.h"

std::shared_ptr<InputManager> InputManagerPtr::inputManager = nullptr;

InputManager::InputManager()
{
}

InputManager::InputManager(std::shared_ptr<VulkanWindow> window)
{
	controller = GameController();
	keyboard = Keyboard();
	mouse = Mouse();
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
	controller.Update();
	keyboard.Update(WindowPtr::GetWindowPtr(), CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera);
	mouse.Update(WindowPtr::GetWindowPtr(), CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera);
}
