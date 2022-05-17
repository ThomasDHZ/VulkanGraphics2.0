#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "IrradiancePipeline.h"
#include "EquirectangularToCubemapPipeline.h"
class EquirectangularToCubemapRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	uint32_t CubeMapSize;
	VkFramebuffer IrradianceMapFrameBuffer;

public:
	EquirectangularToCubemapRenderPass();
	EquirectangularToCubemapRenderPass(uint32_t cubeMapSize);
	~EquirectangularToCubemapRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<EquirectangularToCubemapPipeline> irradiancePipeline;

	void RebuildSwapChain(uint32_t cubeMapSize);

	void Draw();
	void Destroy();
};