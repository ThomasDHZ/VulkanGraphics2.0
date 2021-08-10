#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class SkyBoxFrameBufferRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	SkyBoxFrameBufferRenderingPipeline();
	SkyBoxFrameBufferRenderingPipeline(const VkRenderPass& renderPass);
	~SkyBoxFrameBufferRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};