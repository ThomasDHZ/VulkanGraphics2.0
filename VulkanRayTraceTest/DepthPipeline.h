#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class DepthPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DepthPipeline();
	DepthPipeline(const VkRenderPass& renderPass);
	~DepthPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);

};

