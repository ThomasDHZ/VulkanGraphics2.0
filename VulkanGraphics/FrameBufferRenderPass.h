#pragma once
#include "FrameBufferPipeline.h"
#include "BaseRenderPass.h"

class FrameBufferRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferRenderPass();

	std::shared_ptr<FrameBufferPipeline> frameBufferPipeline;

	void RebuildSwapChain(std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void Draw();
	void Destroy();
};