#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "SSAOPipeline.h"
#include "SSAOBlurPipeline.h"

class SSAORenderPass
{
private:

	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();
	void GenerateKernal(SSAOTextureList& Textures);
	void GenerateNoiseTexture(SSAOTextureList& textures);
	float Lerp(float a, float b, float f);

	std::vector<std::shared_ptr<VulkanBuffer>> SamplePointBufferList;
	std::shared_ptr<Texture2D> NoiseTexture;
public:
	SSAORenderPass();
	SSAORenderPass(SSAOTextureList& Textures);
	~SSAORenderPass();

	static constexpr RenderPassID rendererPassID = SSAO_Renderer;
	int KernalSampleSize = 64;
	float radius = 0.5f;
	float bias = 0.25f;
	std::shared_ptr<RenderedColorTexture> SSAOTexture;
	std::shared_ptr<SSAOPipeline> RasterSSAOPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(SSAOTextureList& Textures);
	void Draw();
	void Destroy();
};
