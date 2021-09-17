#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class WireFramePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	WireFramePipeline();
	WireFramePipeline(const VkRenderPass& renderPass);
	~WireFramePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};