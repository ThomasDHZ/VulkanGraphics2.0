#pragma once
#include "Vulkanengine.h"
#include "InterfaceRenderPass.h"
#include "Mesh.h"
#include "Texture2D.h"

class BaseRenderer
{
protected:

public:
	BaseRenderer();
	~BaseRenderer();

	virtual void RebuildSwapChain() = 0;
	virtual void GUIUpdate() = 0;
	virtual void Destroy() = 0;

	virtual std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

