#pragma once
#include "GraphicsPipeline.h"
#include "RenderedCubeMapDepthTexture.h"

class DepthCubeDebugPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets(std::shared_ptr<RenderedCubeMapDepthTexture> depthTexture);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	DepthCubeDebugPipeline();
	DepthCubeDebugPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapDepthTexture> depthTexture);
	~DepthCubeDebugPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapDepthTexture> depthTexture);
};