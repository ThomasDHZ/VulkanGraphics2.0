#pragma once
#include "GraphicsPipeline.h"
#include "AssetManager.h"

class RenderFrameBufferTexturePipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
public:
	RenderFrameBufferTexturePipeline();
	RenderFrameBufferTexturePipeline(const VkRenderPass& renderPass);
	~RenderFrameBufferTexturePipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass);
};

 