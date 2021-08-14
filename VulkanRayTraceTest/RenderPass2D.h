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
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	RenderPass2D();
	RenderPass2D(std::shared_ptr<VulkanEngine> engine);
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

	void RebuildSwapChain();
	void Draw(RendererID rendererID);
	void Destroy();
};

