#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "AssetManager.h"

class ForwardRenderPass
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
	ForwardRenderPass();
	ForwardRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~ForwardRenderPass();

	VkRenderPass RenderPass;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};