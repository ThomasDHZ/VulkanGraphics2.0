#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"

class BRDFPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorBindings();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BRDFPipeline();
	BRDFPipeline(const VkRenderPass& renderPass);
	~BRDFPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

