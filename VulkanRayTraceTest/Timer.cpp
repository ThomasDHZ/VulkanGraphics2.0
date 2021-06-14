#include "Timer.h"
#include <GLFW/glfw3.h>

Timer::Timer()
{
	LastTime = glfwGetTime();
}

Timer::~Timer()
{
}

float Timer::GetFrameTime()
{
	float oldtime = LastTime;
	LastTime = glfwGetTime();

	float frameTime = LastTime - oldtime;
	return frameTime;
}
