#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderFrameBufferTexturePipeline.h"
#include "Shader2DPipeline.h"
#include "WireFramePipeline.h"

class RenderPass2D
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	RenderPass2D();
	RenderPass2D(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	~RenderPass2D();

	bool WireFrameFlag = false;
	static constexpr RenderPassID rendererPassID = Texture_Renderer;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;

	std::shared_ptr<Shader2DPipeline> TexturePipeline;
	std::shared_ptr<WireFramePipeline> wireFramePipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, RendererID rendererID);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};

