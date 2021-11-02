#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"
#include "RenderedCubeMapDepthTexture.h"

class BlinnPhongPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList);
	void SetUpDescriptorLayout(std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList);
	void SetUpDescriptorSets(std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> ShadowCubeMap);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BlinnPhongPipeline();
	BlinnPhongPipeline(const VkRenderPass& renderPass, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> ShadowCubeMap);
	~BlinnPhongPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> ShadowCubeMap);
};

