#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class ForwardRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	ForwardRenderingPipeline();
	ForwardRenderingPipeline(const VkRenderPass& renderPass);
	~ForwardRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};