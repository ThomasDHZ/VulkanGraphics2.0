#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
class RenderWaterTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, VkRenderPass& renderPass);
public:
	RenderWaterTexturePipeline();
	RenderWaterTexturePipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& renderPass);
	~RenderWaterTexturePipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& renderPass);
};

