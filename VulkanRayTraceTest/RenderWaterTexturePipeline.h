#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
class RenderWaterTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(VkRenderPass& renderPass);
public:
	RenderWaterTexturePipeline();
	RenderWaterTexturePipeline(VkRenderPass& renderPass);
	~RenderWaterTexturePipeline();

	void UpdateGraphicsPipeLine(VkRenderPass& renderPass);
};

