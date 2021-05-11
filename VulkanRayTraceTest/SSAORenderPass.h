#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "SSAOPipeline.h"

class SSAORenderPass
{
private:
	uint32_t KernalSampleSize = 64;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);
	void GenerateKernal(VulkanEngine& engine, SSAOTextureList& Textures);
	void GenerateNoiseTexture(VulkanEngine& engine, SSAOTextureList& textures);
	float Lerp(float a, float b, float f);
	std::vector<VulkanBuffer> SamplePointBufferList;

public:
	SSAORenderPass();
	SSAORenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& Textures);
	~SSAORenderPass();

	static constexpr RenderPassID rendererPassID = SSAO_Renderer;

	std::shared_ptr<RenderedColorTexture> SSAOTexture;
	std::shared_ptr<SSAOPipeline> RasterSSAOPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& Textures);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};
