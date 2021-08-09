#pragma once
#include "Vulkanengine.h"
#include "PBRRenderer.h"
#include "BlinnPhongRasterRenderer.h"
#include "RayTraceRenderer.h"
#include "RayTracePBRRenderer.h"
#include "HybridRenderer.h"
#include "Renderer2D.h"
#include "GUIRenderer.h"

class Renderer
{
private:
	bool RayTraceFlag = false;
	int ActiveRenderer = 0;

	BlinnPhongRasterRenderer blinnPhongRenderer;
	//PBRRenderer pbrRenderer;
	//RayTraceRenderer rayTraceRenderer;
	//RayTracePBRRenderer pbrRayTraceRenderer;
	//HybridRenderer hybridRenderer;
	//Renderer2D renderer2D;
	//GUIRenderer guiRenderer;
	InterfaceRenderPass interfaceRenderPass;

	size_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<VkCommandBuffer> CommandBufferSubmitList;

public:
	std::shared_ptr<AssetManager> assetManager;

	Renderer();
	Renderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPTR);
	~Renderer();

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};