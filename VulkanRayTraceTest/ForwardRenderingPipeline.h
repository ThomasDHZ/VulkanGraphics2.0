#pragma once
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ForwardRenderingPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
public:
	ForwardRenderingPipeline();
	ForwardRenderingPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
	~ForwardRenderingPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
};

