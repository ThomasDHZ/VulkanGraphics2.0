#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "RenderedColorTexture.h"

class brdfRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	brdfRenderingPipeline();
	brdfRenderingPipeline(const VkRenderPass& renderPass);
	~brdfRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

