#pragma once
#include "GraphicsPipeline.h"

class SceneRenderingPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout& DescriptorLayout);
public:
	SceneRenderingPipeline();
	SceneRenderingPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout& DescriptorLayout);
	~SceneRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout& DescriptorLayout);
};
