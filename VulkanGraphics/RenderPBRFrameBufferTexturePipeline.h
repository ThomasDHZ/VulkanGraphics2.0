#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
#include "RenderedCubeMapTexture.h"

class RenderPBRFrameBufferTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	RenderPBRFrameBufferTexturePipeline();
	RenderPBRFrameBufferTexturePipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	~RenderPBRFrameBufferTexturePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
};

