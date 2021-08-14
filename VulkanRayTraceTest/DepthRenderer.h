#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthPipeline.h"
#include "RenderedDepthTexture.h"

class DepthRenderer
{
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthRenderer();
	DepthRenderer(std::shared_ptr<VulkanEngine> engine);
	~DepthRenderer();

	static constexpr RenderPassID rendererPassID = DebugDepth_Renderer;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<DepthPipeline> depthPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain();
	void Draw();
	void Destroy();
};

