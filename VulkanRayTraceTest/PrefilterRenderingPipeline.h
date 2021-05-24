#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class PrefilterRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	PrefilterRenderingPipeline();
	PrefilterRenderingPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	~PrefilterRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	void RecreateRendererTexture(VulkanEngine& engine, glm::vec2 TextureResolution);
};