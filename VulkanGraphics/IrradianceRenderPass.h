#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "IrradiancePipeline.h"

class IrradianceRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	uint32_t CubeMapSize;
	VkFramebuffer IrradianceMapFrameBuffer;

public:
	IrradianceRenderPass();
	IrradianceRenderPass(std::shared_ptr<VulkanEngine> engine);
	~IrradianceRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<RenderedCubeMapTexture> FullRenderedCubeMap;
	std::shared_ptr<IrradiancePipeline> irradiancePipeline;

	void RebuildSwapChain();

	void Draw();
	void Destroy();
};

