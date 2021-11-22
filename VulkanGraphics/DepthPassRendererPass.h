#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "DepthPassPipeline.h"

class DepthPassRendererPass : public BaseRenderPass
{
private:
	static constexpr RendererDrawFlag RendererType = RendererDrawFlag::Renderer_Draw_Shadow_Pass;

	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedDepthTexture> DepthToTexture;
public:
	DepthPassRendererPass();
	DepthPassRendererPass(uint32_t depthTextureSize);
	~DepthPassRendererPass();


	std::vector<std::shared_ptr<RenderedDepthTexture>> DepthTextureList;
	std::shared_ptr<DepthPassPipeline> depthPipeline;

	void RebuildSwapChain(uint32_t depthTextureSize);

	void Draw();
	void Destroy();
};
