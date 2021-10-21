#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"

class ReflectionCubeMapPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
public:
	ReflectionCubeMapPipeline();
	ReflectionCubeMapPipeline(const VkRenderPass& renderPass, float CubeMapSize);
	~ReflectionCubeMapPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, float CubeMapSize);
};