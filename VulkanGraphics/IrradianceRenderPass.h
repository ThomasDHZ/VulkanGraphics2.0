#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "PBRPipeline.h"

class IrradianceRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedColorTexture> ColorTexture;

public:
	IrradianceRenderPass();
	IrradianceRenderPass(std::shared_ptr<VulkanEngine> engine);
	~IrradianceRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<PBRPipeline> pbrPipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};

