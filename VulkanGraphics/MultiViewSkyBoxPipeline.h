#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class MultiViewSkyBoxPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> cubeMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> cubeMapTexture, float CubeMapSize);
public:
	MultiViewSkyBoxPipeline();
	MultiViewSkyBoxPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> cubeMapTexture, float CubeMapSize);
	~MultiViewSkyBoxPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> cubeMapTexture, float CubeMapSize);
};

