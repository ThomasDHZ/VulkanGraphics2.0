#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
class RenderWaterTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void SetUpShaderPipeLine(VulkanEngine& engine, VkRenderPass& renderPass);
public:
	RenderWaterTexturePipeline();
	RenderWaterTexturePipeline(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, VkRenderPass& renderPass);
	~RenderWaterTexturePipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, VkRenderPass& renderPass);
};

