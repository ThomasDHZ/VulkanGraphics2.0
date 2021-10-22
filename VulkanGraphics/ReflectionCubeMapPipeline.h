#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"

class ReflectionCubeMapPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	ReflectionCubeMapPipeline();
	ReflectionCubeMapPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~ReflectionCubeMapPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
};

