#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"

class DepthPassPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DepthPassPipeline();
	DepthPassPipeline(const VkRenderPass& renderPass);
	~DepthPassPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};