#pragma once
#include "FrameBufferPipeline.h"
#include "HybridFrameBufferPipeline.h"
#include "RenderFrameBufferTexturePipeline.h"

class HybridFrameBufferRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	HybridFrameBufferRenderPass();
	HybridFrameBufferRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, HybridFrameBufferTextures& HybridTextures);
	~HybridFrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = FrameBuffer_Renderer;

	std::shared_ptr<HybridFrameBufferPipeline> frameBufferPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, HybridFrameBufferTextures& HybridTextures);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, RendererID rendererID);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};