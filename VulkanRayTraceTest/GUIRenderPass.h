#pragma once
#include "RenderFrameBufferTexturePipeline.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "GUIRenderPipeline.h"
#include "BaseRenderPass.h"

class GUIRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	GUIRenderPass();
	GUIRenderPass(std::shared_ptr<VulkanEngine> engine);
	~GUIRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<GUIRenderPipeline> TextRenderingPipeline;

	void RebuildSwapChain();
	void UpdateSwapChain();
	void Draw();
	void Destroy();
};