#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::BaseRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr)
{
	assetManager = assetManagerPtr;
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
}

void BaseRenderer::GUIUpdate(VulkanEngine& engine)
{
}

void BaseRenderer::Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex, Skybox skybox)
{
}

void BaseRenderer::Destroy(VulkanEngine& engine)
{
}

std::vector<VkCommandBuffer> BaseRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
	return std::vector<VkCommandBuffer>();
}
