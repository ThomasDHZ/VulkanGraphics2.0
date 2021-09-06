#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::~BaseRenderer()
{
}

std::vector<VkCommandBuffer> BaseRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
	return std::vector<VkCommandBuffer>();
}
