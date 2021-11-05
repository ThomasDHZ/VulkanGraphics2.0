#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"

class IrradiancePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<CubeMapTexture> cubeMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, std::shared_ptr<CubeMapTexture> cubeMapTexture, float CubeMapSize);
public:
	IrradiancePipeline();
	IrradiancePipeline(const VkRenderPass& renderPass, std::shared_ptr<CubeMapTexture> cubeMapTexture, float CubeMapSize);
	~IrradiancePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<CubeMapTexture> cubeMapTexture, float CubeMapSize);
};

