#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "BloomPipeline.h"

class BloomRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	BloomRenderPass();
	BloomRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> InputBloomTexture);
	~BloomRenderPass();

	static constexpr RenderPassID rendererPassID = SSAO_Renderer;

	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<BloomPipeline> BloomPipelinePass1;
	std::shared_ptr<BloomPipeline> BloomPipelinePass2;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManage, std::shared_ptr<Texture> InputBloomTexture);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};
