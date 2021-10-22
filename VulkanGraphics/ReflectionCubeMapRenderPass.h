#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"

class ReflectionCubeMapRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;

public:
	ReflectionCubeMapRenderPass();
	ReflectionCubeMapRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~ReflectionCubeMapRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> RenderedBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<BlinnPhongPipeline> blinnphongPipeline;
	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;

	void RebuildSwapChain(std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);

	void Draw();
	void Destroy();
};
