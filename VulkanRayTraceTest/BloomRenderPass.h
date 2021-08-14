#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "BloomPipeline.h"

class BloomRenderPass
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

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<Texture> InputBloomTexture);
	void Draw();
	void Destroy();
};
