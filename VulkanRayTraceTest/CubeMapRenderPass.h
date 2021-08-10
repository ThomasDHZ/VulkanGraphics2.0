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
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	CubeMapRenderPass();
	CubeMapRenderPass(uint32_t CubeMapSize);
	~CubeMapRenderPass();

	static constexpr RenderPassID RendererID = CubeTexture_Renderer;

	uint32_t CubeMapSize;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> BlurredSkyBoxTexture;

	std::shared_ptr<CubeMapRenderingPipeline> CubeMapTexturePipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain();
	void Draw(uint32_t imageIndex);
	void Destroy();
};

