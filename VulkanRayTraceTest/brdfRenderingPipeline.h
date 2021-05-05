#pragma once
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class brdfRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<UniformData<SkyboxUniformBuffer>> sceneData);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	brdfRenderingPipeline();
	brdfRenderingPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<UniformData<SkyboxUniformBuffer>> sceneData, const VkRenderPass& renderPass);
	~brdfRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
};