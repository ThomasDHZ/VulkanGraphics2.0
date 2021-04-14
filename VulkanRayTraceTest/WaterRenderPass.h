#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"

class WaterRenderPass
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, SceneDataUniformBuffer sceneData);
	void SetUpShaderPipeLine(VulkanEngine& renderer);
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	WaterRenderPass();
	WaterRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~WaterRenderPass();

	std::shared_ptr<PerspectiveCamera> ReflectionCam;
	SceneDataUniformBuffer sceneData;

	std::shared_ptr<RenderedColorTexture> ReflectionTexture;
	std::shared_ptr<RenderedColorTexture> RefractionTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void Update(VulkanEngine& engine, AssetManager& assetManager, SceneDataUniformBuffer& copysceneData);
	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};
