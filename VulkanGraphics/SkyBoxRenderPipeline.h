#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class SkyBoxRenderPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, VkSampleCountFlagBits SampleCount);
public:
	SkyBoxRenderPipeline();
	SkyBoxRenderPipeline(const VkRenderPass& renderPass, VkSampleCountFlagBits SampleCount);
	~SkyBoxRenderPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, VkSampleCountFlagBits SampleCount);
};