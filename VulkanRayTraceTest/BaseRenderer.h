#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"

class BaseRenderer
{
protected:
	std::shared_ptr<AssetManager> assetManager;
public:
	BaseRenderer();
	BaseRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BaseRenderer();

	virtual void UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window) = 0;
	virtual void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage) = 0;
	virtual void GUIUpdate(VulkanEngine& engine) = 0;
	virtual void Draw(VulkanEngine& engine, VulkanWindow& window) = 0;
	virtual void Destroy(VulkanEngine& engine) = 0;
};

