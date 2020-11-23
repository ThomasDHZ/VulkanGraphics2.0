#pragma once
#include "GraphicsPipeline.h"

enum PipelineBitFlagsEnum
{
	NormalPipeline = 1 << 0,
	AnimatedPipeline = 1 << 1,
	BloomPipeline = 1 << 2,
	ShadowPipeline = 1 << 3,
	WireFramePipeline = 1 << 4
};

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

