#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedCubeMapDepthTexture.h"
#include "DepthCubeMapPipeline.h"
#include "ReflectionCubeMapPipeline.h"

class ReflectionCubeMapRenderPass : public BaseRenderPass
{
private:
	static constexpr RendererDrawFlag RendererType = RendererDrawFlag::Renderer_Draw_Reflection_Pass;
	std::shared_ptr<CubeSampler> cubeSampler;

	void CreateRenderPass();
	void CreateRendererFramebuffers();

public:
	ReflectionCubeMapRenderPass();
	ReflectionCubeMapRenderPass(uint32_t cubeMapSize, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~ReflectionCubeMapRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeBloom;
	std::shared_ptr<ReflectionCubeMapPipeline> depthCubeMapPipeline;
	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;

	void RebuildSwapChain(uint32_t cubeMapSize, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);

	void Update();
	void Draw();
	void Destroy();
};

