#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "RenderedColorTexture.h"

class HybridFrameBufferPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> RTXShadowTexture, std::shared_ptr<Texture> RTXReflectionTexture, std::shared_ptr<Texture> RTXSSA0Texture, std::shared_ptr<Texture> BloomTexture);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	HybridFrameBufferPipeline();
	HybridFrameBufferPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> RTXShadowTexture, std::shared_ptr<Texture> RTXReflectionTexture, std::shared_ptr<Texture> RTXSSA0Texture, std::shared_ptr<Texture> BloomTexture);
	~HybridFrameBufferPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> RTXShadowTexture, std::shared_ptr<Texture> RTXReflectionTexture, std::shared_ptr<Texture> RTXSSA0Texture, std::shared_ptr<Texture> BloomTexture);
};