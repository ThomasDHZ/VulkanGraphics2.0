#pragma once
#include "VulkanEngine.h"

class Animator2D
{
	private:
		uint32_t FrameCount;
		uint32_t FrameRow;
		uint32_t FrameColumn;
		glm::vec2 FrameSize;
		
		glm::vec2 FrameUV;

	public:
		Animator2D();
		Animator2D(uint32_t frameRow, uint32_t frameColumn, glm::vec2 frameSize);
		~Animator2D();

		int CurrentFrame;
		glm::vec2 GetFrameUVs(int frame);
};

