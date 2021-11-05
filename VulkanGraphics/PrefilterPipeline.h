#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class PrefilterPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> cubeMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
public:
	PrefilterPipeline();
	PrefilterPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> cubeMapTexture, float CubeMapSize);
	~PrefilterPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> cubeMapTexture, float CubeMapSize);
};

