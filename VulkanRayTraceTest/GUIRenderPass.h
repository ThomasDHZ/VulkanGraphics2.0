#pragma once
#include "RenderFrameBufferTexturePipeline.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "GUIRenderPipeline.h"

class GUIRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	GUIRenderPass();
	GUIRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	~GUIRenderPass();

	static constexpr RenderPassID rendererPassID = Texture_Renderer;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<GUIRenderPipeline> TextRenderingPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, RendererID rendererID);
	void Destroy(VulkanEngine& engine);
};