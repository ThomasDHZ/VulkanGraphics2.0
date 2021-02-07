#pragma once
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ForwardRenderingPipeline : public GraphicsPipeline
{
private:
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout& DescriptorLayout);
public:
	ForwardRenderingPipeline();
	ForwardRenderingPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout& DescriptorLayout);
	~ForwardRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, VkDescriptorSetLayout& DescriptorLayout);
};
