#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"

class BlinnPhongPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BlinnPhongPipeline();
	BlinnPhongPipeline(const VkRenderPass& renderPass);
	~BlinnPhongPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

