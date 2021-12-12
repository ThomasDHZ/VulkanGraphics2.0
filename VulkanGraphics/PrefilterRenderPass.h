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

	uint32_t CubeMapMipLevels = 0;

	std::shared_ptr<RenderedCubeMapTexture> DrawToCubeMap;
	VkFramebuffer PrefilterMapFrameBuffer;
	bool firstRun = true;

public:
	PrefilterRenderPass();
	PrefilterRenderPass(std::shared_ptr<Texture> cubeMapTexture, uint32_t cubeMapSize);
	~PrefilterRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedCubeMap;
	std::shared_ptr<PrefilterPipeline> prefilterPipeline;

	void RebuildSwapChain(std::shared_ptr<Texture> cubeMapTexture, uint32_t cubeMapSize);

	void Draw();
	void Destroy();
};