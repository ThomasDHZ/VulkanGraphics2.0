#pragma once
#include "GraphicsPipeline.h"

class ShadowRenderingPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VkDevice device);
	void CreateShaderPipeLine(VkDevice device, const VkRenderPass& renderPass, VkExtent2D extent);
public:
	ShadowRenderingPipeline();
	ShadowRenderingPipeline(VkDevice device, const VkRenderPass& renderPass, VkExtent2D extent);
	~ShadowRenderingPipeline();

	void UpdateGraphicsPipeLine(VkDevice device, const VkRenderPass& renderPass, VkExtent2D extent);
};

