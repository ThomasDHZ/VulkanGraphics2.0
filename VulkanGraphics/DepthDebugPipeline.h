#pragma once
#include "GraphicsPipeline.h"
#include "Texture2D.h"
#include "RenderedDepthTexture.h"

class DepthDebugPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedDepthTexture> depthTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DepthDebugPipeline();
	DepthDebugPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> depthTexture);
	~DepthDebugPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedDepthTexture> depthTexture);
};