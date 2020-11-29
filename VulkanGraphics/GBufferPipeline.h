#pragma once
#include "GraphicsPipeline.h"

class GBufferPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
public:
	GBufferPipeline();
	GBufferPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
	~GBufferPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
};

