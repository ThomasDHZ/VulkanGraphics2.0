#pragma once
#include "Vulkanengine.h"
#include "ImageProcessor.h"
#include "InterfaceRenderPass.h"
#include "Mesh.h"
#include "Texture2D.h"

class BaseRenderer
{
protected:
	bool GUIChangedFlag = false;
	bool ShadowDebugFlag = false;

public:
	BaseRenderer();
	~BaseRenderer();

	virtual void RebuildSwapChain()
	{}
	virtual void GUIUpdate() {}
	virtual void Destroy() {}

	virtual std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

