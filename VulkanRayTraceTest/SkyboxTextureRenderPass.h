#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderTexturePipeline.h"
#include "RenderedCubeMapTexture.h"
#include "PBRIrradiancePipeline.h"

class SkyboxTextureRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);
	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;
public:
	SkyboxTextureRenderPass();
	SkyboxTextureRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<PerspectiveCamera> camera);
	~SkyboxTextureRenderPass();

	static constexpr RenderPassID RendererID = CubeTexture_Renderer;
	SkyboxUniformBuffer sceneData;

	std::shared_ptr<PBRIrradiancePipeline> pbrIrradiancePipeline;
	std::shared_ptr<RenderedCubeMapTexture> CubeMapTexture;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};

