#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"

class PBRPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	PBRPipeline();
	PBRPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
	~PBRPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::shared_ptr<RenderedDepthTexture> ShadowMapTexture);
};

