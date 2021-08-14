#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "DepthDebugPipeline.h"
#include "BaseRenderPass.h"

class DepthDebugRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	DepthDebugRenderPass();
	DepthDebugRenderPass(std::shared_ptr<Texture> DepthTexture);
	~DepthDebugRenderPass();

	std::shared_ptr<RenderedColorTexture> DebugDepthTexture;
	std::shared_ptr<DepthDebugPipeline> DebugDepthPipeline;

	void RebuildSwapChain(std::shared_ptr<Texture> DepthTexture);
	void Draw();
	void Destroy();
};