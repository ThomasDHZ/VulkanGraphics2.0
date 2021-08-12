#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class DebugLightRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DebugLightRenderingPipeline();
	DebugLightRenderingPipeline(const VkRenderPass& renderPass);
	~DebugLightRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};