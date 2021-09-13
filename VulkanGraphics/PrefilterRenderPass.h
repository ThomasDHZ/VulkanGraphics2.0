#pragma once
#include "Vulkanengine.h"
#include "BaseRenderPass.h"
#include "PrefilterRenderingPipeline.h"
#include "RenderedColorTexture.h"
#include "RenderedCubeMapTexture.h"


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
	void Draw(int x);
	void Destroy();
};

