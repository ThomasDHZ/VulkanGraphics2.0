#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"

class BlinnPhongPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BlinnPhongPipeline();
	BlinnPhongPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~BlinnPhongPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
};

