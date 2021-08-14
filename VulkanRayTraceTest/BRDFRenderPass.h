#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RenderedColorTexture.h"
#include "BloomPipeline.h"
#include "brdfRenderingPipeline.h"

class BRDFRenderPass
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

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<Texture> InputBloomTexture);
	void Draw();
	void Destroy();
};
