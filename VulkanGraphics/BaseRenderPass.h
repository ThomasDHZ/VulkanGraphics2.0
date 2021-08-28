#pragma once
#include "Vulkanengine.h"

class BaseRenderPass
{
private:
protected:
	glm::ivec2 RenderPassResolution;

public:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> CommandBuffer;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	BaseRenderPass();
	BaseRenderPass(glm::ivec2 renderPassResolution);
	~BaseRenderPass();
	virtual void Destroy();

	VkCommandBuffer GetCommandBuffer() { return CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]; }
};
