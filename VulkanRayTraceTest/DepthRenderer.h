#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthPipeline.h"
#include "RenderedDepthTexture.h"

class DepthRenderer
{
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	DepthRenderer();
	DepthRenderer(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	~DepthRenderer();

	static constexpr RenderPassID rendererPassID = DebugDepth_Renderer;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<DepthPipeline> depthPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};

