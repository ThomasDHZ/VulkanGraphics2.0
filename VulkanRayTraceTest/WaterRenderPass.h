#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderWaterTexturePipeline.h"
#include "Skybox.h"

class WaterRenderToTextureRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	WaterRenderToTextureRenderPass();
	WaterRenderToTextureRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~WaterRenderToTextureRenderPass();

	static constexpr RenderPassID RendererID = Water_Renderer;

	std::shared_ptr<PerspectiveCamera> TextureCamera;

	std::shared_ptr<SceneDataUniformBuffer> SceneDataBuffer;
	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<RenderWaterTexturePipeline> WaterTexturePipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> WaterSkyboxRenderingPipeline;


	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void Update(VulkanEngine& engine, AssetManager& assetManager, SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera);
	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, Skybox skybox);
	void Destroy(VulkanEngine& engine);
};
