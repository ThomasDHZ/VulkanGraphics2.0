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

class ReflectionCubeMapRenderPass : public BaseRenderPass
{
private:
	std::shared_ptr<CubeSampler> cubeSampler;

	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	ReflectionCubeMapRenderPass();
	ReflectionCubeMapRenderPass(uint32_t cubeMapSize);
	~ReflectionCubeMapRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<DepthCubeMapPipeline> depthCubeMapPipeline;

	void RebuildSwapChain(uint32_t cubeMapSize);

	void Update();
	void Draw();
	void Destroy();
};

