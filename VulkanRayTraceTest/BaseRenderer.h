#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "InterfaceRenderPass.h"
#include "Skybox.h"

class BaseRenderer
{
protected:
	std::shared_ptr<AssetManager> assetManager;

public:
	BaseRenderer();
	BaseRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BaseRenderer();

	virtual void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	virtual void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	virtual void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, Skybox skybox);
	virtual void Destroy(std::shared_ptr<VulkanEngine> engine);

	virtual std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

