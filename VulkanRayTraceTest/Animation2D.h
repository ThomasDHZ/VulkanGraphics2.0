#pragma once
#include "VulkanEngine.h"

typedef glm::vec2 Frame2D;
class Animation2D
{
private:
	uint32_t CurrentFrame;
	float DeltaFrameTime;
	float NextFrame;
	std::vector<Frame2D> FrameList;

public:
	Animation2D();
	Animation2D(std::vector<Frame2D> frameList, float deltaFrameTimeuint32_t, uint32_t StartFrame = 0);

	void Update(float timer);
	uint32_t GetCurrentFrame() { return CurrentFrame; }
	Frame2D GetFrame() { return FrameList[CurrentFrame]; }
};

