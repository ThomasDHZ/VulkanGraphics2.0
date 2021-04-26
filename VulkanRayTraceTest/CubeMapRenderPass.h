#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderTexturePipeline.h"
#include "CubeMapRenderingPipeline.h"
#include "SkyBoxRenderingPipeline.h"
#include "Skybox.h"

class CubeMapRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	CubeMapRenderPass();
	CubeMapRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer);
	~CubeMapRenderPass();

	static constexpr RenderPassID RendererID = CubeTexture_Renderer;
	SceneDataUniformBuffer sceneData;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> CopyTexture;
	std::shared_ptr<RenderedColorTexture> CopyTexture2;
	std::shared_ptr<RenderedColorTexture> CopyTexture3;
	std::shared_ptr<RenderedColorTexture> CopyTexture4;
	std::shared_ptr<RenderedColorTexture> CopyTexture5;
	std::shared_ptr<RenderedColorTexture> CopyTexture6;

	std::shared_ptr<CubeMapRenderingPipeline> CubeMapTexturePipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> skyBoxRenderingPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void Update(VulkanEngine& engine, AssetManager& assetManager, SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera);
	void UpdateSwapChain(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, Skybox& skybox);
	void Destroy(VulkanEngine& engine);
};

