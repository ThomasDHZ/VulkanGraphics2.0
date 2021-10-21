#pragma once
#include "BaseRenderPass.h"
#include "DepthCubeMapPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "ReflectionCubeMapPipeline.h"
#include "SkyBoxRenderPipeline.h"

class ReflectionCubeMapRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	ReflectionCubeMapRenderPass();
	ReflectionCubeMapRenderPass(uint32_t cubeMapSize);
	~ReflectionCubeMapRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;

	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;
	std::shared_ptr<ReflectionCubeMapPipeline> reflectionCubeMapPipeline;

	void RebuildSwapChain(uint32_t cubeMapSize);

	void Update();
	void Draw();
	void Destroy();
};

