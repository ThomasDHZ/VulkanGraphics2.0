#pragma once
#include "RenderFrameBufferTexturePipeline.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "GUIRenderPipeline.h"

class GUIRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	GUIRenderPass();
	GUIRenderPass(std::shared_ptr<VulkanEngine> engine);
	~GUIRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<GUIRenderPipeline> TextRenderingPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain();
	void UpdateSwapChain();
	void Draw();
	void Destroy();
};