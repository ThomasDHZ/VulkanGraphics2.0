#pragma once
#include "GraphicsPipeline.h"

class SceneRender2DPipeline : public GraphicsPipeline
{
private:
	void CreateDescriptorSetLayout(VulkanEngine& renderer);
	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
public:
	SceneRender2DPipeline();
	SceneRender2DPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
	~SceneRender2DPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
};
