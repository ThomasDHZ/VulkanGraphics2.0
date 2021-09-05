#pragma once
#include "GraphicsPipeline.h"

class FrameBufferRenderingPipeline : public GraphicsPipeline
{
private:
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout layout);
public:
	FrameBufferRenderingPipeline();
	FrameBufferRenderingPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout layout);
	~FrameBufferRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout layout);
};
