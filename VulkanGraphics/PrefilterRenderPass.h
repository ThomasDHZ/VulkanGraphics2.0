#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "CubeMapRenderingPipeline.h"
#include "Skybox.h"
#include "RenderedCubeMapTexture.h"
#include "PrefilterRenderingPipeline.h"
#include "BaseRenderPass.h"

class PrefilterRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	PrefilterRenderPass();
	PrefilterRenderPass(uint32_t CubeMapSize);
	~PrefilterRenderPass();

	uint32_t CubeMapSize;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> BlurredSkyBoxTexture;

	std::shared_ptr<PrefilterRenderingPipeline> prefilterRenderingPipeline;

	void RebuildSwapChain();
	void Draw();
	void Destroy();
};

