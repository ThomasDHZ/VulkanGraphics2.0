#pragma once
#include "Vulkanengine.h"
#include "InterfaceRenderPass.h"
//#include "BlinnPhongRasterPass.h"
//#include "BlinnPhongRasterRenderer.h"
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
	int ActiveRenderer = 0;

	//BlinnPhongRasterRenderer blinnPhongRenderer;
	//PBRRenderer pbrRenderer;
	//RayTraceRenderer rayTraceRenderer;
	//RayTracePBRRenderer pbrRayTraceRenderer;
	//HybridRenderer hybridRenderer;
	//Renderer2D renderer2D;
	//GUIRenderer guiRenderer;

	//BlinnPhongRasterRenderer BlinnRenderer;
	InterfaceRenderPass interfaceRenderPass;
//	RayTraceRenderer rayTraceRenderer;
	std::vector<VkCommandBuffer> CommandBufferSubmitList;
public:
	RendererManager();
	RendererManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, TextureManager textureManager, ModelRenderManager modelrenderer, std::vector<Model> modelList, std::shared_ptr<SceneDataStruct> scene);
	~RendererManager();

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};