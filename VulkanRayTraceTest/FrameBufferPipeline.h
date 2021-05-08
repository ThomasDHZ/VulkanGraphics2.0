#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "RenderedColorTexture.h"

class FrameBufferPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
public:
	FrameBufferPipeline();
	FrameBufferPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& renderer, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
};