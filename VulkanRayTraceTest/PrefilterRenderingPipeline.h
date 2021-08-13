#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class PrefilterRenderingPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	PrefilterRenderingPipeline();
	PrefilterRenderingPipeline(const VkRenderPass& renderPass);
	~PrefilterRenderingPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
	void RecreateRendererTexture(glm::vec2 TextureResolution);
};