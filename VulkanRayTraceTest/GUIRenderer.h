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
	GUIRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~GUIRenderer();

	static constexpr RendererID rendererID = RendererID::GUI_Renderer;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);

	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};
