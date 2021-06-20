#pragma once
#include "VulkanEngine.h"
#include "Timer.h"

typedef glm::vec2 Frame2D;
class Animation2D
{
private:
	uint32_t CurrentFrame;
	float CurrentFrameTime;
	float FrameHoldTime;
	std::vector<Frame2D> FrameList;

public:
	Animation2D();
	Animation2D(std::vector<Frame2D> frameList, float frameHoldTime, uint32_t StartFrame = 0);

	void Update(std::shared_ptr<Timer> timer, glm::ivec2 SingleSpriteSize, uint32_t SpriesInSpriteSheet);
	uint32_t GetCurrentFrame() { return CurrentFrame; }
	Frame2D GetFrame() { return FrameList[CurrentFrame]; }
};

