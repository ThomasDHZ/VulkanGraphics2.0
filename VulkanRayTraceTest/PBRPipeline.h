#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class PBRPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	PBRPipeline();
	PBRPipeline(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, const VkRenderPass& renderPass);
	~PBRPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass) override;
};