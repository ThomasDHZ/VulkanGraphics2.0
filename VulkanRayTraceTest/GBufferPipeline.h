#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class GBufferPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	GBufferPipeline();
	GBufferPipeline(const VkRenderPass& renderPass);
	~GBufferPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};