#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedGBufferNormalTexture.h"
#include "RenderedGBufferAlbedoTexture.h"
#include "RenderedGBufferPositionTexture.h"
#include "GBufferPipeline.h"
#include "RenderedColorTexture.h"
#include "SSAOPipeline.h"
#include "SSAOBlurPipeline.h"
#include "RenderedSSAOTexture.h"

class GBufferRenderPass
{
private:

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);

public:
	GBufferRenderPass();
	GBufferRenderPass(VulkanEngine& engine);
	~GBufferRenderPass();

	VkRenderPass RenderPass;
	VkRenderPass SSAORenderPass;
	VkRenderPass SSAOBlurRenderPass;

	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::vector<VkFramebuffer> SSAOSwapChainFramebuffers;
	std::vector<VkFramebuffer> SSAOBlurSwapChainFramebuffers;

	std::shared_ptr<RenderedGBufferPositionTexture> GPositionTexture;
	std::shared_ptr<RenderedGBufferNormalTexture> GNormalTexture;
	std::shared_ptr<RenderedGBufferAlbedoTexture> GAlbedoTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<RenderedSSAOTexture> SSAOTexture;
	std::shared_ptr<RenderedSSAOTexture> SSAOBlurTexture;

	std::shared_ptr<GBufferPipeline> gBufferPipeline;
	std::shared_ptr<GBufferPipeline> AnimatedGBufferPipeline;
	std::shared_ptr<SSAOPipeline> ssaoPipeline;
	std::shared_ptr<SSAOBlurPipeline> ssaoBlurPipeline;

	void UpdateSwapChain(VulkanEngine& engine);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};