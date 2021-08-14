#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "SSAOBlurPipeline.h"

class SSAOBlurRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	SSAOBlurRenderPass();
	SSAOBlurRenderPass(std::shared_ptr<Texture> SSAOTexture);
	~SSAOBlurRenderPass();

	static constexpr RenderPassID rendererPassID = SSAO_Renderer;

	std::shared_ptr<RenderedColorTexture> SSAOBlurTexture;
	std::shared_ptr<SSAOBlurPipeline> RasterSSAOPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<Texture> SSAOTexture);
	void Draw();
	void Destroy();
};
