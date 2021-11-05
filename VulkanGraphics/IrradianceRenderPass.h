#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "IrradiancePipeline.h"

class IrradianceRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	IrradianceRenderPass();
	IrradianceRenderPass(std::shared_ptr<Texture> cubeMapTexture, uint32_t cubeMapSize);
	~IrradianceRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<IrradiancePipeline> irradiancePipeline;

	void RebuildSwapChain(std::shared_ptr<Texture> cubeMapTexture, uint32_t cubeMapSize);

	void Draw();
	void Destroy();
};

