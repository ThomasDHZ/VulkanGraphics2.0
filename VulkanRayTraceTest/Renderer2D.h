#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "LightPathRenderer.h"
#include "RenderPass2D.h"

class Renderer2D : public BaseRenderer
{
private:
	RenderPass2D FrameBufferTextureRenderer;
	BloomRenderPass BloomRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
public:
	Renderer2D();
	Renderer2D(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~Renderer2D();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

