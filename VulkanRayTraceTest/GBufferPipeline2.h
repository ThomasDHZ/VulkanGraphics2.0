#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class GBufferPipeline2 : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	GBufferPipeline2();
	GBufferPipeline2(const VkRenderPass& renderPass);
	~GBufferPipeline2();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};