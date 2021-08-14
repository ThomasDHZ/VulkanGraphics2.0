#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "InterfaceRenderPass.h"
#include "Skybox.h"

class BaseRenderer
{
protected:

public:
	BaseRenderer();
	~BaseRenderer();

	virtual void RebuildSwapChain();
	virtual void GUIUpdate();
	virtual void Draw(Skybox skybox);
	virtual void Destroy();

	virtual std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

