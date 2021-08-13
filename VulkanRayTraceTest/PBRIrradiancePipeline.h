#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class PBRIrradiancePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, RenderPassID RendererID);
public:
	PBRIrradiancePipeline();
	PBRIrradiancePipeline(const VkRenderPass& renderPass, RenderPassID RendererID);
	~PBRIrradiancePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, RenderPassID RendererID);
};