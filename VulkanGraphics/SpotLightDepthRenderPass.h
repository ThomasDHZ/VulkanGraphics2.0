#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "DepthPassPipeline.h"

class SpotLightDepthRenderPass : public BaseRenderPass
{
private:
	static constexpr RendererDrawFlag RendererType = RendererDrawFlag::Renderer_Draw_Shadow_Pass;

	void CreateRenderPass();
	void CreateRendererFramebuffers();

	std::shared_ptr<RenderedDepthTexture> DepthToTexture;
public:
	SpotLightDepthRenderPass();
	SpotLightDepthRenderPass(uint32_t depthTextureSize);
	~SpotLightDepthRenderPass();

	std::vector<std::shared_ptr<RenderedDepthTexture>> DepthTextureList;
	std::shared_ptr<DepthPassPipeline> depthPipeline;

	void RebuildSwapChain(uint32_t depthTextureSize);

	void Draw();
	void Destroy();
};

