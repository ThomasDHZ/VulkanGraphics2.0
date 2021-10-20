#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "PBRPipeline.h"

class PBRRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;

public:
	PBRRenderPass();
	PBRRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~PBRRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> RenderedBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<PBRPipeline> pbrPipeline;
	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;

	void RebuildSwapChain(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);

	void Draw();
	void Destroy();
};

