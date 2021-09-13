#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "CubeMapRenderingPipeline.h"
#include "Skybox.h"
#include "RenderedCubeMapTexture.h"
#include "BaseRenderPass.h"

class CubeMapRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	CubeMapRenderPass();
	CubeMapRenderPass(uint32_t CubeMapSize);
	~CubeMapRenderPass();

	uint32_t CubeMapSize;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> BlurredSkyBoxTexture;

	std::shared_ptr<CubeMapRenderingPipeline> CubeMapTexturePipeline;

	void RebuildSwapChain();
	void Draw();
	void Destroy();
};

