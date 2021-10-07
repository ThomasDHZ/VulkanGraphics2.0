#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"

class BRDFPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BRDFPipeline();
	BRDFPipeline(const VkRenderPass& renderPass);
	~BRDFPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

