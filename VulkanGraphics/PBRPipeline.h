#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"

class PBRPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	PBRPipeline();
	PBRPipeline(const VkRenderPass& renderPass);
	~PBRPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

