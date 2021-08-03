#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class GBufferPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
public:
	GBufferPipeline();
	GBufferPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	~GBufferPipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
};