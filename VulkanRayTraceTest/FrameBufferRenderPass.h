#pragma once
#include "FrameBufferPipeline.h"
#include "BaseRenderPass.h"

class FrameBufferRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(glm::ivec2 renderPassResolution, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferRenderPass();

	std::shared_ptr<FrameBufferPipeline> frameBufferPipeline;

	void RebuildSwapChain(std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void Draw();
	void Destroy();
};