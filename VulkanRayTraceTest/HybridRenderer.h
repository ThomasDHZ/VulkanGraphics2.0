#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"
#include "HybridFrameBufferRenderPass.h"
#include "DeferredRenderPass.h"

class HybridRenderer : public BaseRenderer
{
private:
	bool UseFrameBuffer = true;
	GBufferRenderPass FrameBufferTextureRenderer;
	HybridFrameBufferRenderPass FrameBufferRenderer;
public:
	HybridRenderer();
	HybridRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~HybridRenderer();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

