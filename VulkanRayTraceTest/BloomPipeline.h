#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"
#include "RenderedColorTexture.h"

class BloomPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> BloomTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	BloomPipeline();
	BloomPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> BloomTexture);
	~BloomPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> BloomTexture);
};

