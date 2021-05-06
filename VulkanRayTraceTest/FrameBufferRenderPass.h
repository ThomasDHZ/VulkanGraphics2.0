#pragma once
#include "FrameBufferPipeline.h"

class FrameBufferRenderPass
{
private:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	~FrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = Forward_Renderer;

	std::shared_ptr<FrameBufferPipeline> frameBufferPipeline;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(VulkanEngine& engine, VkCommandBuffer commandbuffer, uint32_t index);
	void Destroy(VulkanEngine& engine);
};