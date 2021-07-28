#pragma once
#include "VulkanEngine.h"

struct GuiRectangle
{
	glm::vec2 UpperLeftVertex;
	glm::vec2 UpperRightVertex;
	glm::vec2 LowerLeftVertex;
	glm::vec2 LowerRightVertex;

	GuiRectangle()
	{

	}

	GuiRectangle(glm::vec2 upperLeftVertex, glm::vec2 upperRightVertex, glm::vec2 lowerLeftVertex, glm::vec2 lowerRightVertex)
	{
		UpperLeftVertex = upperLeftVertex;
		UpperRightVertex = upperRightVertex;
		LowerLeftVertex = lowerLeftVertex;
		LowerRightVertex = lowerRightVertex;
	}
};

