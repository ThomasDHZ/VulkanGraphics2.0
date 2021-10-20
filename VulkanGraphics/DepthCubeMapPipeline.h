#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class DepthCubeMapPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<CubeSampler> cubeSampler);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
public:
	DepthCubeMapPipeline();
	DepthCubeMapPipeline(const VkRenderPass& renderPass, float CubeMapSize, std::shared_ptr<CubeSampler> cubeSampler);
	~DepthCubeMapPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, float CubeMapSize, std::shared_ptr<CubeSampler> cubeSampler);
};

