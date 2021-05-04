#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::BaseRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr)
{
	assetManager = assetManagerPtr;
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
}

void BaseRenderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
}

void BaseRenderer::GUIUpdate(VulkanEngine& engine)
{
}

void BaseRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex, Skybox skybox)
{
}

void BaseRenderer::Destroy(VulkanEngine& engine)
{
}

std::vector<VkCommandBuffer> BaseRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
	return std::vector<VkCommandBuffer>();
}
