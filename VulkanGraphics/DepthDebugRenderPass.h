#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "DepthDebugPipeline.h"
#include "RenderedColorTexture.h"

class DepthDebugRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();

public:
	DepthDebugRenderPass();
	DepthDebugRenderPass(std::shared_ptr<RenderedDepthTexture> depthTexture);
	~DepthDebugRenderPass();

	std::shared_ptr<RenderedColorTexture> DebugTexture;
	std::shared_ptr<DepthDebugPipeline> depthDebugPipeline;

	void RebuildSwapChain(std::shared_ptr<RenderedDepthTexture> depthTexture);

	void Draw();
	void Destroy();
};
