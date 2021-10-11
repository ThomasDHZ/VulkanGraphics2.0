#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"

class BaseRenderPass
{
private:
protected:
public:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> CommandBuffer;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	glm::ivec2 RenderPassResolution;

	BaseRenderPass();
	~BaseRenderPass();
	virtual void Destroy();

	VkCommandBuffer GetCommandBuffer() { return CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]; }
};
