#pragma once
#include "GraphicsPipeline.h"

class ForwardRenderering2DPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
public:
	ForwardRenderering2DPipeline();
	ForwardRenderering2DPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
	~ForwardRenderering2DPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
};

