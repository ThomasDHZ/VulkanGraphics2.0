#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class SkyBoxRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	SkyBoxRenderingPipeline();
	SkyBoxRenderingPipeline(const VkRenderPass& renderPass);
	~SkyBoxRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};