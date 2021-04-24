#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class CubeMapRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<UniformData<SkyboxUniformBuffer>> sceneData);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	CubeMapRenderingPipeline();
	CubeMapRenderingPipeline(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<UniformData<SkyboxUniformBuffer>> sceneData, const VkRenderPass& renderPass);
	~CubeMapRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
};