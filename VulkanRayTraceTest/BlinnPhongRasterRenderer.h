#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"

class BlinnPhongRasterRenderer : public BaseRenderer
{
private:
	FrameBufferTextureRenderPass FrameBufferTextureRenderer;
public:
	BlinnPhongRasterRenderer();
	BlinnPhongRasterRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~BlinnPhongRasterRenderer();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	ForwardRenderPass forwardRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

