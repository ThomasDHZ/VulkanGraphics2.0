#pragma once
#include "BaseRenderPass.h"
#include "DepthCubeMapPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedCubeMapDepthTexture.h"

class DepthCubeMapRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	VkFramebuffer IrradianceMapFrameBuffer;

public:
	DepthCubeMapRenderPass();
	DepthCubeMapRenderPass(uint32_t cubeMapSize);
	~DepthCubeMapRenderPass();

	std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap;
	std::shared_ptr<DepthCubeMapPipeline> depthCubeMapPipeline;

	void RebuildSwapChain(uint32_t cubeMapSize);

	void Draw();
	void Destroy();
};

