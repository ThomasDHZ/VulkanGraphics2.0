#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthDebugPipeline.h"

class DepthDebugRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	DepthDebugRenderPass();
	DepthDebugRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> DepthTexture);
	~DepthDebugRenderPass();

	static constexpr RenderPassID rendererPassID = DebugDepth_Renderer;

	std::shared_ptr<RenderedColorTexture> DebugDepthTexture;
	std::shared_ptr<DepthDebugPipeline> DebugDepthPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManage, std::shared_ptr<Texture> DepthTexture);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};