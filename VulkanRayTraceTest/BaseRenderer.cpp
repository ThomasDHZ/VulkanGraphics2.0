#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::RebuildSwapChain()
{
}

void BaseRenderer::GUIUpdate()
{
}

void BaseRenderer::Draw(Skybox skybox)
{
}

void BaseRenderer::Destroy()
{
}

std::vector<VkCommandBuffer> BaseRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
	return std::vector<VkCommandBuffer>();
}
