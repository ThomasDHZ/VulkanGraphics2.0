#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderPBRFrameBufferTexturePipeline.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"
#include "RenderedCubeMapTexture.h"
#include "RenderPBRFrameBufferTexturePipeline.h"
#include "BaseRenderPass.h"

class PBRFrameBufferTextureRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	PBRFrameBufferTextureRenderPass();
	PBRFrameBufferTextureRenderPass(std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	~PBRFrameBufferTextureRenderPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<RenderPBRFrameBufferTexturePipeline> PBRTexturePipeline;
	std::shared_ptr<SkyBoxFrameBufferRenderingPipeline> skyBoxRenderingPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::vector<VkCommandBuffer> CommandBuffer;

	void RebuildSwapChain(std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void UpdateSwapChain(std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void Draw();
	void Destroy();
};

