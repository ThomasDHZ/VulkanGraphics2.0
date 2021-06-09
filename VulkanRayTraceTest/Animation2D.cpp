#include "Animation2D.h"

Animation2D::Animation2D()
{
	DeltaFrameTime = 0;
	CurrentFrame = 0;
	NextFrame = 0;
}

Animation2D::Animation2D(std::vector<Frame2D> frameList, float deltaFrameTime, uint32_t StartFrame)
{
	FrameList = frameList;
	DeltaFrameTime = deltaFrameTime;
	CurrentFrame = StartFrame;
	NextFrame = DeltaFrameTime;
}

void Animation2D::Update(float timer)
{
	if (timer >= NextFrame)
	{
		CurrentFrame += 1;
		if (CurrentFrame > FrameList.size())
		{
			CurrentFrame = 0;
		}
		NextFrame += timer + DeltaFrameTime;
	}
}
