#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

class WaterSurfacePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	WaterSurfacePipeline();
	WaterSurfacePipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
	~WaterSurfacePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> reflectionTexture, std::shared_ptr<Texture> refractionTexture);
};