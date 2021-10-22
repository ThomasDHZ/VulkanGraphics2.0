#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "ReflectionCubeMapPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "RenderedCubeMapTexture.h"

class ReflectionCubeMapRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedCubeMapTexture> ColorTexture;
	std::shared_ptr<RenderedCubeMapTexture> BloomTexture;

public:
	ReflectionCubeMapRenderPass();
	ReflectionCubeMapRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~ReflectionCubeMapRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> RenderedBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<ReflectionCubeMapPipeline> reflectionPipeline;
	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;

	void RebuildSwapChain(std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);

	void Draw();
	void Destroy();
};

