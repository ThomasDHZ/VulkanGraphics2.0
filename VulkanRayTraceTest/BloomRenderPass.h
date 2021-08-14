#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "BloomPipeline.h"
#include "BaseRenderPass.h"

class BloomRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	BloomRenderPass();
	BloomRenderPass(std::shared_ptr<Texture> InputBloomTexture);
	~BloomRenderPass();

	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<BloomPipeline> BloomPipelinePass1;
	std::shared_ptr<BloomPipeline> BloomPipelinePass2;

	void RebuildSwapChain(std::shared_ptr<Texture> InputBloomTexture);
	void Draw();
	void Destroy();
};
