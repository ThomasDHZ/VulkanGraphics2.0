#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
class RenderWaterTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(VulkanEngine& engine, VkRenderPass& renderPass);
public:
	RenderWaterTexturePipeline();
	RenderWaterTexturePipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& renderPass);
	~RenderWaterTexturePipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& renderPass);
};

