#pragma once
#include "GraphicsPipeline.h"

class BloomPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
public:
	BloomPipeline();
	BloomPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass);
	~BloomPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
};
