#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthDebugPipeline.h"

class DepthDebugRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthDebugRenderPass();
	DepthDebugRenderPass(std::shared_ptr<Texture> DepthTexture);
	~DepthDebugRenderPass();

	std::shared_ptr<RenderedColorTexture> DebugDepthTexture;
	std::shared_ptr<DepthDebugPipeline> DebugDepthPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<Texture> DepthTexture);
	void Draw();
	void Destroy();
};