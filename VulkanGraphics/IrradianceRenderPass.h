#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "PBRPipeline.h"
#include "IrradiancePipeline.h"
#include "RenderedCubeMapTexture.h"

class IrradianceRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	uint32_t CubeMapSize;
	std::shared_ptr<RenderedColorTexture> ColorTexture;

public:
	IrradianceRenderPass();
	IrradianceRenderPass(std::shared_ptr<VulkanEngine> engine);
	~IrradianceRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> BlurredSkyBoxTexture;

	std::shared_ptr<IrradiancePipeline> irradiancePipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};

