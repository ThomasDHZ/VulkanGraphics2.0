#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"
#include "Texture2DArray.h"

class BlinnPhongPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<Texture2DArray> ShadowMapTextureList);
	void SetUpDescriptorLayout(std::shared_ptr<Texture2DArray> ShadowMapTextureList);
	void SetUpDescriptorSets(std::shared_ptr<Texture2DArray> ShadowMapTextureList);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BlinnPhongPipeline();
	BlinnPhongPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture2DArray> ShadowMapTextureList);
	~BlinnPhongPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture2DArray> ShadowMapTextureList);
};

