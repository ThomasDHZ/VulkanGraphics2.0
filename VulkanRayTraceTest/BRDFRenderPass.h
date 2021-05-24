#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "BloomPipeline.h"
#include "brdfRenderingPipeline.h"

class BRDFRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	BRDFRenderPass();
	BRDFRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> InputBloomTexture);
	~BRDFRenderPass();

	static constexpr RenderPassID rendererPassID = SSAO_Renderer;

	std::shared_ptr<RenderedColorTexture> BRDFTexture;
	std::shared_ptr<brdfRenderingPipeline> BRDFPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManage, std::shared_ptr<Texture> InputBloomTexture);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};
