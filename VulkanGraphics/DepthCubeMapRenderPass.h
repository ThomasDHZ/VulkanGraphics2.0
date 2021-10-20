#pragma once
#include "BaseRenderPass.h"
#include "DepthCubeMapPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedCubeMapDepthTexture.h"

class DepthCubeMapRenderPass : public BaseRenderPass
{
private:
	std::shared_ptr<CubeSampler> cubeSampler;

	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthCubeMapRenderPass();
	DepthCubeMapRenderPass(uint32_t cubeMapSize);
	~DepthCubeMapRenderPass();

	std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap;
	std::shared_ptr<DepthCubeMapPipeline> depthCubeMapPipeline;

	void RebuildSwapChain(uint32_t cubeMapSize);

	void Update();
	void Draw();
	void Destroy();
};

