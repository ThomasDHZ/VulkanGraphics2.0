#pragma once
#include "VulkanEngine.h"
#include "Timer.h"

typedef glm::vec2 Frame2D;
typedef glm::ivec2 FrameOffset;
class Animation2D
{
private:
	uint32_t CurrentFrame;
	Frame2D CurrentFrameUV;
	float CurrentFrameTime;
	float FrameHoldTime;
	std::vector<FrameOffset> FrameList;

public:
	Animation2D();
	Animation2D(std::vector<FrameOffset> frameList, float frameHoldTime, uint32_t StartFrame = 0);

	void Update(std::shared_ptr<Timer> timer, glm::vec2 spriteUVSize, uint32_t SpriesInSpriteSheet, bool FlipSpriteX);
	uint32_t GetCurrentFrame() { return CurrentFrame; }
	Frame2D GetFrame() { return CurrentFrameUV; }
};

