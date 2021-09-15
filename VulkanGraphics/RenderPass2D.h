#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "Shader2DPipeline.h"
#include "BaseRenderPass.h"
#include "WireFramePipeline.h"

class RenderPass2D : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	RenderPass2D();
	RenderPass2D(std::shared_ptr<VulkanEngine> engine);
	~RenderPass2D();

	bool WireFrameFlag = false;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;

	std::shared_ptr<Shader2DPipeline> TexturePipeline;
	std::shared_ptr<WireFramePipeline> wireFramePipeline;

	void RebuildSwapChain();
	void Draw();
	void Destroy();
};

