#pragma once
#include "FrameBufferPipeline.h"
#include "HybridFrameBufferPipeline.h"
#include "RenderFrameBufferTexturePipeline.h"

class HybridFrameBufferRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	HybridFrameBufferRenderPass();
	HybridFrameBufferRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> RTXShadowTexture, std::shared_ptr<Texture> RTXReflectionTexture, std::shared_ptr<Texture> RTXSSA0Texture, std::shared_ptr<Texture> BloomTexture);
	~HybridFrameBufferRenderPass();

	static constexpr RenderPassID rendererPassID = FrameBuffer_Renderer;

	std::shared_ptr<HybridFrameBufferPipeline> frameBufferPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> RTXShadowTexture, std::shared_ptr<Texture> RTXReflectionTexture, std::shared_ptr<Texture> RTXSSA0Texture, std::shared_ptr<Texture> BloomTexture);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t index, RendererID rendererID);
	void Destroy(VulkanEngine& engine);
};