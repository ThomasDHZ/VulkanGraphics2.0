#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "DepthPassPipeline.h"

class DepthPassRendererPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthPassRendererPass();
	DepthPassRendererPass(uint32_t depthTextureSize);
	~DepthPassRendererPass();

	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DepthPassPipeline> depthPipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};
