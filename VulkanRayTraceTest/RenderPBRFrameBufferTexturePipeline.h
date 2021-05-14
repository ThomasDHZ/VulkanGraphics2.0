#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"
#include "RenderedCubeMapTexture.h"

class RenderPBRFrameBufferTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	RenderPBRFrameBufferTexturePipeline();
	RenderPBRFrameBufferTexturePipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
	~RenderPBRFrameBufferTexturePipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> irradianceMap, std::shared_ptr<RenderedCubeMapTexture> prefilterMap, std::shared_ptr<Texture> brdfLUT);
};

