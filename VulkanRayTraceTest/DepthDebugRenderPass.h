#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthDebugPipeline.h"

class DepthDebugRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	DepthDebugRenderPass();
	DepthDebugRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> DepthTexture);
	~DepthDebugRenderPass();

	static constexpr RenderPassID rendererPassID = DebugDepth_Renderer;

	std::shared_ptr<RenderedColorTexture> DebugDepthTexture;
	std::shared_ptr<DepthDebugPipeline> DebugDepthPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManage, std::shared_ptr<Texture> DepthTexture);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};