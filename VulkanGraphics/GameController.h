#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include <iostream>

class GameController
{
private:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	GLFWgamepadstate state;
public:
	GameController();
	~GameController();
	void Update();
};