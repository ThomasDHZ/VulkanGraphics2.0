#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderFrameBufferTexturePipeline.h"
#include "SkyBoxRenderingPipeline.h"
#include "RenderPBRFrameBufferTexturePipeline.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"

class FrameBufferTextureRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	FrameBufferTextureRenderPass();
	FrameBufferTextureRenderPass(std::shared_ptr<AssetManager> assetManager);
	~FrameBufferTextureRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<RenderFrameBufferTexturePipeline> TexturePipeline;
	std::shared_ptr<SkyBoxFrameBufferRenderingPipeline> skyBoxRenderingPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain();
	void UpdateSwapChain();
	void Draw();
	void Destroy();
};

