#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "BloomRenderPass.h"

class BlinnPhongRasterRenderer : public BaseRenderer
{
private:
	bool UseFrameBuffer = true;
	FrameBufferTextureRenderPass FrameBufferTextureRenderer;
	BloomRenderPass BloomRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
public:
	BlinnPhongRasterRenderer();
	BlinnPhongRasterRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BlinnPhongRasterRenderer();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	ForwardRenderPass forwardRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

