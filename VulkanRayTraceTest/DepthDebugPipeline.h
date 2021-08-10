#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "GraphicsPipeline.h"

struct ConstDepthDebugProperites
{
	alignas(4) float NearPlain = 1.0f;
	alignas(4) float FarPlain = 7.5f;
};

class DepthDebugPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<Texture> DepthTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DepthDebugPipeline();
	DepthDebugPipeline(const VkRenderPass& renderPass, std::shared_ptr<Texture> DepthTexture);
	~DepthDebugPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<Texture> DepthTexture);
};

