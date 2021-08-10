#pragma once
#include "FrameBufferPipeline.h"

class FrameBufferRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = FrameBuffer_Renderer;

	std::shared_ptr<FrameBufferPipeline> frameBufferPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void Draw(uint32_t index);
	void Destroy();
};