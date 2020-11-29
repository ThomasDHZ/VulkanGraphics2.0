#pragma once
#include "GraphicsPipeline.h"

class SSAOBlurPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
public:
	SSAOBlurPipeline();
	SSAOBlurPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass);
	~SSAOBlurPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
};

