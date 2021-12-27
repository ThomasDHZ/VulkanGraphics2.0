#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "LightDebugPipeline.h"

class BlinnPhongRasterPass : public BaseRenderPass
{
private:
	static constexpr RendererDrawFlag RendererType = RendererDrawFlag::Renderer_Draw_Main_Pass;
	
	void CreateRenderPass();
	void CreateRendererFramebuffers();

	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;

public:
	BlinnPhongRasterPass();
	BlinnPhongRasterPass(std::shared_ptr<VulkanEngine> engine, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);
	~BlinnPhongRasterPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> RenderedBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<BlinnPhongPipeline> blinnphongPipeline;
	std::shared_ptr<LightDebugPipeline> debugLightPipeline;
	std::shared_ptr<SkyBoxRenderPipeline> skyboxPipeline;

	void RebuildSwapChain(std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);

	void Draw();
	void Destroy();
};

