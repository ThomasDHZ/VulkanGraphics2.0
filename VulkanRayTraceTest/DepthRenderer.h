#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthPipeline.h"
#include "RenderedDepthTexture.h"
#include "BaseRenderPass.h"

class DepthRenderer : public BaseRenderPass
{
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthRenderer();
	DepthRenderer(std::shared_ptr<VulkanEngine> engine);
	~DepthRenderer();

	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<DepthPipeline> depthPipeline;

	void RebuildSwapChain();
	void Draw();
	void Destroy();
};

