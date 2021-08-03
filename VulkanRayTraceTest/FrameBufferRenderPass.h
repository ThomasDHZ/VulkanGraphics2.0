#pragma once
#include "FrameBufferPipeline.h"

class FrameBufferRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = FrameBuffer_Renderer;

	std::shared_ptr<FrameBufferPipeline> frameBufferPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t index);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};