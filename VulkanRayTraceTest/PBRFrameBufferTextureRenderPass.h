#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderFrameBufferTexturePipeline.h"
#include "SkyBoxRenderingPipeline.h"
#include "RenderPBRFrameBufferTexturePipeline.h"
#include "SkyBoxFrameBufferRenderingPipeline.h"

class PBRFrameBufferTextureRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	PBRFrameBufferTextureRenderPass();
	PBRFrameBufferTextureRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	~PBRFrameBufferTextureRenderPass();

	static constexpr RenderPassID rendererPassID = Texture_Renderer;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<RenderPBRFrameBufferTexturePipeline> PBRTexturePipeline;
	std::shared_ptr<SkyBoxFrameBufferRenderingPipeline> skyBoxRenderingPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void UpdateSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, RendererID rendererID);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};

