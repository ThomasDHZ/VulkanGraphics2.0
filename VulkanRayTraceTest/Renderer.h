#pragma once
#include "VulkanEngine.h"
#include "PBRRenderer.h"
#include "BlinnPhongRasterRenderer.h"
#include "RayTraceRenderer.h"
#include "RayTracePBRRenderer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "HybridRenderer.h"

class Renderer
{
private:
	int ActiveRenderer = 0;
	Keyboard keyboard;
	Mouse mouse;

	BlinnPhongRasterRenderer blinnPhongRenderer;
	PBRRenderer pbrRenderer;
	RayTraceRenderer rayTraceRenderer;
	RayTracePBRRenderer pbrRayTraceRenderer;
	HybridRenderer hybridRenderer;
	InterfaceRenderPass interfaceRenderPass;

	size_t currentFrame = 0;

	bool UpdateRenderer = false;
	bool framebufferResized = false;

	std::vector<VkCommandBuffer> CommandBufferSubmitList;

public:
	std::shared_ptr<AssetManager> assetManager;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPTR);
	~Renderer();

	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window);
	void Destroy(VulkanEngine& engine);
};