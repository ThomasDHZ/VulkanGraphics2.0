#pragma once
#include <chrono>

class Timer
{
private:
	float LastTime;
public:
	Timer();
	~Timer();

	float GetFrameTime();
};

