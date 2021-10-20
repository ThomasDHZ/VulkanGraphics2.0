#pragma once
#include "BaseRenderPass.h"
#include "DepthCubeDebugPipeline.h"
#include "RenderedColorTexture.h"
#include "RenderedCubeMapTexture.h"

class DepthCubeDebugRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthCubeDebugRenderPass();
	DepthCubeDebugRenderPass(std::shared_ptr<RenderedCubeMapDepthTexture> depthTexture);
	~DepthCubeDebugRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<DepthCubeDebugPipeline> depthCubeMapPipeline;

	void RebuildSwapChain(std::shared_ptr<RenderedCubeMapDepthTexture> depthTexture);

	void Draw();
	void Destroy();
};

