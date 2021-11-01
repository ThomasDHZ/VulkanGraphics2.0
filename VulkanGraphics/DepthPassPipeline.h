#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "SceneData.h"

class DepthPassPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<LightSampler> cubeSampler);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DepthPassPipeline();
	DepthPassPipeline(const VkRenderPass& renderPass, std::shared_ptr<LightSampler> cubeSampler);
	~DepthPassPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<LightSampler> cubeSampler);
};