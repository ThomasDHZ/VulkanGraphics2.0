#pragma once
#include "FrameBufferPipeline.h"
#include "HybridFrameBufferPipeline.h"
#include "RenderFrameBufferTexturePipeline.h"
#include "BaseRenderPass.h"

class HybridFrameBufferRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	HybridFrameBufferRenderPass();
	HybridFrameBufferRenderPass(HybridFrameBufferTextures& HybridTextures);
	~HybridFrameBufferRenderPass();

	std::shared_ptr<HybridFrameBufferPipeline> frameBufferPipeline;

	void RebuildSwapChain(HybridFrameBufferTextures& HybridTextures);
	void Draw();
	void Destroy();
};