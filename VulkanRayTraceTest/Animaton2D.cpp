#include "Animaton2D.h"
 
Animator2D::Animator2D()
{
}

Animator2D::Animator2D(uint32_t frameRow, uint32_t frameColumn, glm::vec2 frameSize)
{
	FrameCount = frameRow * frameColumn;
	FrameRow = frameRow;
	FrameColumn = frameColumn;
	FrameSize = frameSize;
	CurrentFrame = 0;
	FrameUV = FrameSize;
}

Animator2D::~Animator2D()
{
}

glm::vec2 Animator2D::GetFrameUVs(int frame)
{
	const uint32_t x = frame % FrameRow;
	const uint32_t y = frame / FrameRow;
	frame + (FrameColumn * frame);
	FrameUV.x = x * FrameSize.x;
	FrameUV.y = y * FrameSize.x;
	return FrameUV;
}
