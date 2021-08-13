#pragma once
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "FontTexture.h"

class GUIRenderPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	GUIRenderPipeline();
	GUIRenderPipeline(const VkRenderPass& renderPass);
	~GUIRenderPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

