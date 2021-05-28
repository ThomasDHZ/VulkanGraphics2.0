#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class GBufferPipeline2 : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	GBufferPipeline2();
	GBufferPipeline2(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	~GBufferPipeline2();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
};