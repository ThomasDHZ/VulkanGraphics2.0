#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
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
	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;

	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};

