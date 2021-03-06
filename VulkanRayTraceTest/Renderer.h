#pragma once
#include "VulkanEngine.h"
#include "PBRRenderer.h"
#include "BlinnPhongRasterRenderer.h"
#include "RayTraceRenderer.h"
#include "RayTracePBRRenderer.h"
#include "HybridRenderer.h"
#include "Renderer2D.h"

class Renderer
{
private:
	bool RayTraceFlag = false;
	int ActiveRenderer = 0;

	BlinnPhongRasterRenderer blinnPhongRenderer;
	PBRRenderer pbrRenderer;
	RayTraceRenderer rayTraceRenderer;
	//RayTracePBRRenderer pbrRayTraceRenderer;
	HybridRenderer hybridRenderer;
	Renderer2D renderer2D;
	InterfaceRenderPass interfaceRenderPass;

	size_t currentFrame = 0;

	bool UpdateRenderer = false;
	bool framebufferResized = false;

	std::vector<VkCommandBuffer> CommandBufferSubmitList;

public:
	std::shared_ptr<AssetManager> assetManager;

	Renderer();
	Renderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPTR);
	~Renderer();

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	void Update(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(VulkanEngine& engine);
};