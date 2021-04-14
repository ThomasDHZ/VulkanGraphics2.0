#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedGBufferAlbedoTexture.h"
#include "RenderedGBufferNormalTexture.h"
#include "RenderedGBufferPositionTexture.h"
#include "AssetManager.h"

class DeferredRenderPass
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void SetUpShaderPipeLine(VulkanEngine& renderer);
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	DeferredRenderPass();
	DeferredRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~DeferredRenderPass();

	struct GBufferPass
	{
		std::shared_ptr<RenderedGBufferPositionTexture> GPositionTexture;
		std::shared_ptr<RenderedGBufferAlbedoTexture> GAlbedoTexture;
		std::shared_ptr<RenderedGBufferNormalTexture> GNormalTexture;
		std::shared_ptr<RenderedColorTexture> GBloomTexture;
		std::shared_ptr<RenderedDepthTexture> DepthTexture;

		std::vector<VkFramebuffer> SwapChainFramebuffers;
		VkRenderPass RenderPass = VK_NULL_HANDLE;
		VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
		VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
		VkPipeline ShaderPipeline = VK_NULL_HANDLE;
	} gBufferRenderPass;

	struct DeferredPass
	{
		std::vector<VkFramebuffer> SwapChainFramebuffers;

		VkRenderPass RenderPass = VK_NULL_HANDLE;
		VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
		VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
		VkPipeline ShaderPipeline = VK_NULL_HANDLE;
	} deferredRenderPass;

	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);
};
