#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
#include "RenderedCubeMapTexture.h"

class RenderPBRFrameBufferTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
public:
	RenderPBRFrameBufferTexturePipeline();
	RenderPBRFrameBufferTexturePipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	~RenderPBRFrameBufferTexturePipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
};

