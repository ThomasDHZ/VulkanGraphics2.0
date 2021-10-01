#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"

class IrradiancePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	IrradiancePipeline();
	IrradiancePipeline(const VkRenderPass& renderPass);
	~IrradiancePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

