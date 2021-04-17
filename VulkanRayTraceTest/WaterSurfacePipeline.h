#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
class WaterSurfacePipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<Texture> reflectionTexture);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	WaterSurfacePipeline();
	WaterSurfacePipeline(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture);
	~WaterSurfacePipeline();

	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
	void Destroy(VulkanEngine& engine);
};