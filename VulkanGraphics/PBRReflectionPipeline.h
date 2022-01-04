#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "SceneData.h"

class PBRReflectionPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorBindings(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::shared_ptr<CubeSampler> cubeSampler, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	PBRReflectionPipeline();
	PBRReflectionPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::shared_ptr<CubeSampler> cubeSampler, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);
	~PBRReflectionPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::shared_ptr<CubeSampler> cubeSampler, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);
};

