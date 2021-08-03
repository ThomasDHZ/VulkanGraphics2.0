#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class GBufferPipeline2 : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
public:
	GBufferPipeline2();
	GBufferPipeline2(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	~GBufferPipeline2();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
};