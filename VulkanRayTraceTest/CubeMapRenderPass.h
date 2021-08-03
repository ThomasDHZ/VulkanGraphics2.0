#pragma once
#include "Vulkanengine.h"
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
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	CubeMapRenderPass();
	CubeMapRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t CubeMapSize);
	~CubeMapRenderPass();

	static constexpr RenderPassID RendererID = CubeTexture_Renderer;

	uint32_t CubeMapSize;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> BlurredSkyBoxTexture;

	std::shared_ptr<CubeMapRenderingPipeline> CubeMapTexturePipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};

