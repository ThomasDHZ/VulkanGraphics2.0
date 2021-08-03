#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "LightPathRenderer.h"

class BlinnPhongRasterRenderer : public BaseRenderer
{
private:
	FrameBufferTextureRenderPass FrameBufferTextureRenderer;
	BloomRenderPass BloomRenderer;
	DepthDebugRenderPass DebugDepthRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
	DepthRenderer lightPathRenderer;
public:
	BlinnPhongRasterRenderer();
	BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BlinnPhongRasterRenderer();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

