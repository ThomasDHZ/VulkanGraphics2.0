#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "brdfRenderingPipeline.h"
#include "BaseRenderPass.h"

class BRDFRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	BRDFRenderPass();
	BRDFRenderPass(std::shared_ptr<Texture> InputBloomTexture);
	~BRDFRenderPass();

	std::shared_ptr<RenderedColorTexture> BRDFTexture;
	std::shared_ptr<brdfRenderingPipeline> BRDFPipeline;

	void RebuildSwapChain(std::shared_ptr<Texture> InputBloomTexture);
	void Draw();
	void Destroy();
};
