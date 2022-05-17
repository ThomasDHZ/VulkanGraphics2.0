#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class EquirectangularToCubemapPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
public:
	EquirectangularToCubemapPipeline();
	EquirectangularToCubemapPipeline(const VkRenderPass& renderPass, float CubeMapSize);
	~EquirectangularToCubemapPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
};

