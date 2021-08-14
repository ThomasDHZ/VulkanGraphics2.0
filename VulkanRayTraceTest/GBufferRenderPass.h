#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedGBufferAlbedoTexture.h"
#include "RenderedGBufferNormalTexture.h"
#include "RenderedGBufferPositionTexture.h"
#include "AssetManager.h"
#include "GBufferPipeline.h"
#include "DeferredSkyboxPipeline.h"
#include "BaseRenderPass.h"

class GBufferRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	GBufferRenderPass();
	GBufferRenderPass(std::shared_ptr<VulkanEngine> engine);
	~GBufferRenderPass();

	std::shared_ptr<RenderedGBufferPositionTexture> GPositionTexture;
	std::shared_ptr<RenderedGBufferAlbedoTexture> GAlbedoTexture;
	std::shared_ptr<RenderedGBufferNormalTexture> GNormalTexture;
	//std::shared_ptr<RenderedGBufferPositionTexture> GTangentTexture;
	//std::shared_ptr<RenderedGBufferPositionTexture> GBiTangentTexture;
	std::shared_ptr<RenderedColorTexture> GBloomTexture;
	std::shared_ptr<RenderedColorTexture> NormalMapTexture;
	std::shared_ptr<RenderedColorTexture> SpecularMapTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<GBufferPipeline> gBufferPipeline;

	void RebuildSwapChain();
	void Draw();
	void Destroy();
};
