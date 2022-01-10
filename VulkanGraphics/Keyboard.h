#pragma once
#include <GLFW\glfw3.h>
#include <memory>
#include "Camera.h"

class Keyboard
{
private:
	bool KeyPressed[350];
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	static Keyboard keyboardListener;

public:
	static Keyboard keyboard();
	static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	

	void Update(GLFWwindow* window, std::shared_ptr<Camera> camera);
	bool IsKeyPressed(int key) { return KeyPressed[key] = GLFW_PRESS; }
	bool IsKeyReleased(int key) { return  KeyPressed[key] = GLFW_RELEASE; };
};