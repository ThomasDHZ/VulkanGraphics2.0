#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "SSAOPipeline.h"
#include "SSAOBlurPipeline.h"
#include "BaseRenderPass.h"

class SSAORenderPass : public BaseRenderPass
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

	int KernalSampleSize = 64;
	float radius = 0.5f;
	float bias = 0.25f;
	std::shared_ptr<RenderedColorTexture> SSAOTexture;
	std::shared_ptr<SSAOPipeline> RasterSSAOPipeline;

	void RebuildSwapChain(SSAOTextureList& Textures);
	void Draw();
	void Destroy();
};
