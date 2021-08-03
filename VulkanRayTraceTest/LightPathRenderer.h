#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "DepthRenderer.h"

class LightPathRenderer : public BaseRenderer
{
private:
	DepthRenderer lightRenderPass;
	DepthDebugRenderPass DebugDepthRenderer;
public:
	LightPathRenderer();
	LightPathRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~LightPathRenderer();

	static constexpr RendererID rendererID = RendererID::LightPath_Renderer;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

