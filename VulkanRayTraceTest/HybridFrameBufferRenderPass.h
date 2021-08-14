#pragma once
#include "FrameBufferPipeline.h"
#include "HybridFrameBufferPipeline.h"
#include "RenderFrameBufferTexturePipeline.h"

class HybridFrameBufferRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	HybridFrameBufferRenderPass();
	HybridFrameBufferRenderPass(HybridFrameBufferTextures& HybridTextures);
	~HybridFrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = FrameBuffer_Renderer;

	std::shared_ptr<HybridFrameBufferPipeline> frameBufferPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(HybridFrameBufferTextures& HybridTextures);
	void Draw(RendererID rendererID);
	void Destroy();
};