#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "SSAOBlurPipeline.h"

class SSAOBlurRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	SSAOBlurRenderPass();
	SSAOBlurRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> SSAOTexture);
	~SSAOBlurRenderPass();

	static constexpr RenderPassID rendererPassID = SSAO_Renderer;

	std::shared_ptr<RenderedColorTexture> SSAOBlurTexture;
	std::shared_ptr<SSAOBlurPipeline> RasterSSAOPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManage, std::shared_ptr<Texture> SSAOTexture);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};
