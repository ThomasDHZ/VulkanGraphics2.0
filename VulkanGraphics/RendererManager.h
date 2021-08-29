#pragma once
#include "Vulkanengine.h"
#include "InterfaceRenderPass.h"
#include "BlinnPhongRasterPass.h"
#include "BlinnPhongRasterRenderer.h"
#include "FrameBufferRenderPass.h"
//#include "PBRRenderer.h"
//#include "BlinnPhongRasterRenderer.h"
#include "RayTraceRenderer.h"
//#include "RayTracePBRRenderer.h"
//#include "HybridRenderer.h"
//#include "Renderer2D.h"
//#include "GUIRenderer.h"

class RendererManager
{
private:
	uint32_t currentFrame = 0;
	uint32_t FirstSixFrames = 0;
	bool WaitSixFramesFlag = true;
	bool framebufferResized = false;

	std::vector<Vertex> vertices = 
	{
	   {{-0.5f, -0.5f, 0.0f}, { 0.0f }, {1.0f, 0.0f, 0.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
	   {{0.5f, -0.5f, 0.0f}, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
	   {{0.5f, 0.5f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
	   {{-0.5f, 0.5f, 0.0f}, { 0.0f }, {1.0f, 1.0f, 1.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},

	   {{-0.5f, -0.5f, -0.5f}, { 0.0f }, {1.0f, 0.0f, 0.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
	   {{0.5f, -0.5f, -0.5f}, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
	   {{0.5f, 0.5f, -0.5f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
	   {{-0.5f, 0.5f, -0.5f}, { 0.0f }, {1.0f, 1.0f, 1.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }}
	};

	std::vector<uint32_t> indices = {
	   0, 1, 2, 2, 3, 0,
	   4, 5, 6, 6, 7, 4
	};

	//BlinnPhongRasterRenderer blinnPhongRenderer;
	//PBRRenderer pbrRenderer;
	RayTraceRenderer rayTraceRenderer;
	//RayTracePBRRenderer pbrRayTraceRenderer;
	//HybridRenderer hybridRenderer;
	//Renderer2D renderer2D;
	//GUIRenderer guiRenderer;

	BlinnPhongRasterRenderer BlinnRenderer;
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