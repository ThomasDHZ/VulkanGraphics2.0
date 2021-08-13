#pragma once
#include "Vulkanengine.h"
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class SSAOBlurPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> SSAOTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	SSAOBlurPipeline();
	SSAOBlurPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> SSAOTexture);
	~SSAOBlurPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> SSAOTexture);
};

