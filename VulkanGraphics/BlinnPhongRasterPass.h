#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"

class BlinnPhongRasterPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;

public:
	BlinnPhongRasterPass();
	BlinnPhongRasterPass(std::shared_ptr<VulkanEngine> engine);
	~BlinnPhongRasterPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> RenderedBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<BlinnPhongPipeline> blinnphongPipeline;
	std::shared_ptr<SkyBoxFrameBufferRenderingPipeline> skyboxPipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};

