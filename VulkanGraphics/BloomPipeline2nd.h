#pragma once
#include "GraphicsPipeline.h"

class BloomPipeline2nd : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
public:
	BloomPipeline2nd();
	BloomPipeline2nd(VulkanEngine& renderer, const VkRenderPass& renderPass);
	~BloomPipeline2nd();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
};

