#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "InterfaceRenderPass.h"
#include "Skybox.h"

class BaseRenderer
{
protected:
	std::shared_ptr<AssetManager> assetManager;
	std::vector<VkCommandBuffer> CommandBufferSubmitList;
	InterfaceRenderPass interfaceRenderPass;

public:
	BaseRenderer();
	BaseRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BaseRenderer();

	virtual void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	virtual void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	virtual void GUIUpdate(VulkanEngine& engine);
	virtual void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex, Skybox skybox);
	virtual void Destroy(VulkanEngine& engine);

	virtual std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

