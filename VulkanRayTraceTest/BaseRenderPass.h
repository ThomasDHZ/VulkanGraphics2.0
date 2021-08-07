#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "Skybox.h"
#include "GlobalPtr.h"

class BaseRenderPass
{
private:
protected:
public:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	BaseRenderPass();
	~BaseRenderPass();
	virtual void Destroy();
};

