#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "InterfaceRenderPass.h"
#include "Skybox.h"

class BaseRenderer
{
protected:
	std::shared_ptr<AssetManager> assetManager;

public:
	BaseRenderer();
	BaseRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BaseRenderer();

	virtual void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	virtual void GUIUpdate(VulkanEngine& engine);
	virtual void Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex, Skybox skybox);
	virtual void Destroy(VulkanEngine& engine);

	virtual std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

