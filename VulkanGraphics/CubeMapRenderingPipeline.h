#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class CubeMapRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	CubeMapRenderingPipeline();
	CubeMapRenderingPipeline(const VkRenderPass& renderPass);
	~CubeMapRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};