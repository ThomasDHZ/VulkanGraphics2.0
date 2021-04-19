#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class WaterSurfacePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	WaterSurfacePipeline();
	WaterSurfacePipeline(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
	~WaterSurfacePipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
};