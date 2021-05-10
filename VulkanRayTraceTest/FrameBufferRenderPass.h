#pragma once
#include "FrameBufferPipeline.h"

class FrameBufferRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = FrameBuffer_Renderer;

	std::shared_ptr<FrameBufferPipeline> frameBufferPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t index, RendererID rendererID);
	void Destroy(VulkanEngine& engine);
};