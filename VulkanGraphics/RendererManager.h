#pragma once
#include "Vulkanengine.h"
#include "InterfaceRenderPass.h"
#include "BlinnPhongRasterPass.h"
#include "BlinnPhongRasterRenderer.h"
#include "FrameBufferRenderPass.h"

//#include "BlinnPhongRasterRenderer.h"
#include "RayTraceRenderer.h"
//#include "RayTracePBRRenderer.h"
//#include "HybridRenderer.h"
#include "Renderer2D.h"
#include "PBRRenderer.h"
//#include "GUIRenderer.h"

class RendererManager
{
private:
	int ActiveRenderer = 0;
	bool GUIChanged = false;
	//BlinnPhongRasterRenderer blinnPhongRenderer;
	//RayTracePBRRenderer pbrRayTraceRenderer;
	//HybridRenderer hybridRenderer;
	//Renderer2D renderer2D;
	//GUIRenderer guiRenderer;

	BlinnPhongRasterRenderer BlinnRenderer;
	RayTraceRenderer rayTraceRenderer;
	Renderer2D renderer2D;
	PBRRenderer pbrRenderer;
	InterfaceRenderPass interfaceRenderPass;

	std::vector<VkCommandBuffer> CommandBufferSubmitList;
public:
	RendererManager();
	RendererManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	~RendererManager();

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};