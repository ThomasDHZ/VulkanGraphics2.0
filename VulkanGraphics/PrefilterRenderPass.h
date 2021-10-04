#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "PrefilterPipeline.h"

class PrefilterRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	uint32_t CubeMapSize;
	VkFramebuffer IrradianceMapFrameBuffer;

public:
	PrefilterRenderPass();
	PrefilterRenderPass(std::shared_ptr<VulkanEngine> engine);
	~PrefilterRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<PrefilterPipeline> prefilterPipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};