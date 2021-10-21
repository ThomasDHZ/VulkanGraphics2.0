#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"

class DebugLightPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DebugLightPipeline();
	DebugLightPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~DebugLightPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
};

