#pragma once
#include "GraphicsPipeline.h"

class SSAOPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
public:
	SSAOPipeline();
	SSAOPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass);
	~SSAOPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
};

