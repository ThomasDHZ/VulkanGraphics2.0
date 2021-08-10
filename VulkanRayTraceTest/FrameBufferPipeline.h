#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "RenderedColorTexture.h"

class FrameBufferPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	FrameBufferPipeline();
	FrameBufferPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
	~FrameBufferPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> RenderedTexture, std::shared_ptr<Texture> BloomTexture);
};