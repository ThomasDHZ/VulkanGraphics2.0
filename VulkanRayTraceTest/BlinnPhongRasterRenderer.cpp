#include "BlinnPhongRasterRenderer.h"

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr)
{
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
}

void BlinnPhongRasterRenderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
}

void BlinnPhongRasterRenderer::GUIUpdate(VulkanEngine& engine)
{
}

void BlinnPhongRasterRenderer::Draw(VulkanEngine& engine, VulkanWindow& window)
{
}

void BlinnPhongRasterRenderer::Destroy(VulkanEngine& engine)
{
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
	return std::vector<VkCommandBuffer>();
}
