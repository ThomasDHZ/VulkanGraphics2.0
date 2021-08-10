#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::BaseRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr)
{
	assetManager = assetManagerPtr;
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
}

void BaseRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
}

void BaseRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, Skybox skybox)
{
}

void BaseRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
}

std::vector<VkCommandBuffer> BaseRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
	return std::vector<VkCommandBuffer>();
}
