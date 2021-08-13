#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class Shader2DPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	Shader2DPipeline();
	Shader2DPipeline(const VkRenderPass& renderPass);
	~Shader2DPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

