#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class LightDebugPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorBindings();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	LightDebugPipeline();
	LightDebugPipeline(const VkRenderPass& renderPass);
	~LightDebugPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

