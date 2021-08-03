#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "RenderedColorTexture.h"

struct HybridFrameBufferTextures
{
	std::shared_ptr<Texture> PositionTexture;
	std::shared_ptr<Texture> AlebdoTexture;
	std::shared_ptr<Texture> NormalTexture;
	//std::shared_ptr<Texture> TangentTexture;
	//std::shared_ptr<Texture> BiTangentTexture;
	std::shared_ptr<Texture> ShadowTexture;
	std::shared_ptr<Texture> ReflectionTexture; 
	std::shared_ptr<Texture> SSA0Texture; 
	std::shared_ptr<Texture> SkyBoxTexture;
	std::shared_ptr<Texture> BloomTexture;
	std::shared_ptr<Texture> NormalMapTexture;
	std::shared_ptr<Texture> SpecularMapTexture;
	std::shared_ptr<Texture> ReflectionMapTexture;
	std::shared_ptr<Texture> TBNTangentTexture;
	std::shared_ptr<Texture> TBNBiTangentTexture;
	std::shared_ptr<Texture> TBNNormalMapTexture;
};

class HybridFrameBufferPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, HybridFrameBufferTextures& HybridTextures);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
public:


	HybridFrameBufferPipeline();
	HybridFrameBufferPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, HybridFrameBufferTextures& HybridTextures);
	~HybridFrameBufferPipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, HybridFrameBufferTextures& HybridTextures);
};