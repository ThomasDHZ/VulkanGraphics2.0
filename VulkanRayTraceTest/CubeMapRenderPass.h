#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "CubeMapRenderingPipeline.h"
#include "SkyBoxRenderingPipeline.h"
#include "Skybox.h"
#include "RenderedCubeMapTexture.h"

class CubeMapRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	CubeMapRenderPass();
	CubeMapRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t CubeMapSize);
	~CubeMapRenderPass();

	static constexpr RenderPassID RendererID = CubeTexture_Renderer;

	uint32_t CubeMapSize;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> BlurredSkyBoxTexture;

	std::shared_ptr<CubeMapRenderingPipeline> CubeMapTexturePipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};

