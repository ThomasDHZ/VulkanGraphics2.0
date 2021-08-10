#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderedCubeMapTexture.h"
#include "PBRIrradiancePipeline.h"

class SkyboxTextureRenderPass
{
private:
	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);
	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;
public:
	SkyboxTextureRenderPass();
	SkyboxTextureRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<PerspectiveCamera> camera);
	~SkyboxTextureRenderPass();

	static constexpr RenderPassID RendererID = CubeTexture_Renderer;
	SkyboxUniformBuffer sceneData;

	std::shared_ptr<PBRIrradiancePipeline> pbrIrradiancePipeline;
	std::shared_ptr<RenderedCubeMapTexture> CubeMapTexture;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void UpdateSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};

