#include "Animation2D.h"

Animation2D::Animation2D()
{
	CurrentFrameTime = 0.0f;
	CurrentFrame = 0.0f;
	FrameHoldTime = 0.0f;
}

Animation2D::Animation2D(std::vector<FrameOffset> frameList, float frameHoldTime, uint32_t StartFrame)
{
	FrameList = frameList;
	CurrentFrame = StartFrame;
	CurrentFrameTime = 0.0f;
	FrameHoldTime = frameHoldTime;
}

void Animation2D::Update(std::shared_ptr<Timer> timer, glm::vec2 spriteUVSize, uint32_t SpriesInSpriteSheet, bool FlipSpriteX)
{
	CurrentFrameTime += timer->GetFrameTime();
	while (CurrentFrameTime >= FrameHoldTime)
	{
		CurrentFrame += 1;
		if (CurrentFrame > FrameList.size() - 1)
		{
			CurrentFrame = 0;
		}
		CurrentFrameTime -= FrameHoldTime;
	}

	if (!FlipSpriteX)
	{
		CurrentFrameUV = Frame2D(spriteUVSize.x * FrameList[CurrentFrame].x, 0.0f);
	}
	else
	{
		CurrentFrameUV = Frame2D(spriteUVSize.x * ((SpriesInSpriteSheet - 1) - FrameList[CurrentFrame].x), 0.0f);
	}
}
