#pragma once
#include "BaseRenderer.h"
#include "FrameBufferRenderPass.h"
#include "PBRRayTraceRenderPass.h"

class PBRRayTraceRenderer : public BaseRenderer
{
private:
	std::shared_ptr<AssetManager> assetManager;
	FrameBufferRenderPass FrameBufferRenderer;
public:
	PBRRayTraceRenderer();
	PBRRayTraceRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~PBRRayTraceRenderer();

	static constexpr RendererID rendererID = RendererID::PBR_RayTrace_Renderer;

	PBRRayTraceRenderPass pbrRayTraceRenderPass;
	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

