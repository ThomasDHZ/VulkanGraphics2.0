#pragma once
#include "GraphicsPipeline.h"
#include "SceneData.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"

class ReflectionCubeMapPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedDepthTexture> ShadowMapTexture, std::shared_ptr<CubeSampler> cubeSampler);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
public:
	ReflectionCubeMapPipeline();
	ReflectionCubeMapPipeline(const VkRenderPass& renderPass, float CubeMapSize, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture, std::shared_ptr<CubeSampler> cubeSampler);
	~ReflectionCubeMapPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, float CubeMapSize, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture, std::shared_ptr<CubeSampler> cubeSampler);
};

