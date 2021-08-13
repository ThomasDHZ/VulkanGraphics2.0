#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class DeferredSkyboxPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DeferredSkyboxPipeline();
	DeferredSkyboxPipeline(const VkRenderPass& renderPass);
	~DeferredSkyboxPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};