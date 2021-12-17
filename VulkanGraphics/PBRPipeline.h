#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "RenderedCubeMapDepthTexture.h"

class PBRPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorBindings(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	PBRPipeline();
	PBRPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap);
	~PBRPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap);
};

