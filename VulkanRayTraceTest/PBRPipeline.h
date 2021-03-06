#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class PBRPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	PBRPipeline();
	PBRPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	~PBRPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
};