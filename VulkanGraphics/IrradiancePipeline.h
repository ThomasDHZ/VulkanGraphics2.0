#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class IrradiancePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
public:
	IrradiancePipeline();
	IrradiancePipeline(const VkRenderPass& renderPass, float CubeMapSize);
	~IrradiancePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
};

