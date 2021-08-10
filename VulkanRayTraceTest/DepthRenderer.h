#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthPipeline.h"
#include "RenderedDepthTexture.h"

class DepthRenderer
{
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	DepthRenderer();
	DepthRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	~DepthRenderer();

	static constexpr RenderPassID rendererPassID = DebugDepth_Renderer;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<DepthPipeline> depthPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};

