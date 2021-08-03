#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class WaterSurfacePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
public:
	WaterSurfacePipeline();
	WaterSurfacePipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
	~WaterSurfacePipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
};