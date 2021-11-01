#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "DepthPassPipeline.h"
#include "Texture2DArray.h"

class DepthPassRendererPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<LightSampler> cubeSampler;

	std::shared_ptr<RenderedDepthTexture> DepthToTexture;
public:
	DepthPassRendererPass();
	DepthPassRendererPass(uint32_t depthTextureSize);
	~DepthPassRendererPass();


	std::vector<std::shared_ptr<RenderedDepthTexture>> DepthTextureList;
	std::shared_ptr<Texture2DArray> LightDepthMaps;
	std::shared_ptr<DepthPassPipeline> depthPipeline;

	void Update();
	void RebuildSwapChain(uint32_t depthTextureSize);

	void Draw();
	void Destroy();
};
