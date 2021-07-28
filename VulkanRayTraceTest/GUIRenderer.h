#pragma once
#include "BaseRenderer.h"
#include "GUIRenderPass.h"
#include "FontTexture.h"
#include "FrameBufferTextureRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "DepthRenderer.h"

class GUIRenderer : public BaseRenderer
{
private:
	GUIRenderPass TextRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
public:
	GUIRenderer();
	GUIRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~GUIRenderer();

	static constexpr RendererID rendererID = RendererID::GUI_Renderer;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(VulkanEngine& engine);

	void Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};
