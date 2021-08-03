#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class CubeMapRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
public:
	CubeMapRenderingPipeline();
	CubeMapRenderingPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
	~CubeMapRenderingPipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass);
};