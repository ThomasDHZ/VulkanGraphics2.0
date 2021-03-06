#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedGBufferAlbedoTexture.h"
#include "RenderedGBufferNormalTexture.h"
#include "RenderedGBufferPositionTexture.h"
#include "AssetManager.h"
#include "GBufferPipeline.h"
#include "DeferredSkyboxPipeline.h"
#include "GBufferPipeline2.h"
#include "RenderedGTBNTexture.h"

class GBufferRenderPass2
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	GBufferRenderPass2();
	GBufferRenderPass2(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	~GBufferRenderPass2();

	static constexpr RenderPassID RendererID = Deferred_Renderer;
	static constexpr RenderPassID rendererPassID = Texture_Renderer;

	std::shared_ptr<RenderedGBufferPositionTexture> GPositionTexture;
	std::shared_ptr<RenderedGBufferAlbedoTexture> GAlbedoTexture;
	std::shared_ptr<RenderedGBufferNormalTexture> GNormalTexture;
	std::shared_ptr<RenderedGTBNTexture> GTBN_TangentTexture;
	std::shared_ptr<RenderedGTBNTexture> GTBN_BiTangentTexture;
	std::shared_ptr<RenderedGTBNTexture> GTBN_NormalTexture;
	std::shared_ptr<RenderedColorTexture> NormalMapTexture;
	std::shared_ptr<RenderedColorTexture> SpecularMapTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<GBufferPipeline2> gBufferPipeline;
	std::shared_ptr<DeferredSkyboxPipeline> skyBoxRenderingPipeline;

	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};
