#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"
#include "RenderedCubeMapDepthTexture.h"

class BlinnPhongPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorBindings(std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BlinnPhongPipeline();
	BlinnPhongPipeline(const VkRenderPass& renderPass, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap);
	~BlinnPhongPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap);
};

