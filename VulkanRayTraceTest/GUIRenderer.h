#pragma once
#include "BaseRenderer.h"
#include "TextRenderPass.h"
#include "FontTexture.h"

class GUIRenderer : public BaseRenderer
{
private:
	std::shared_ptr<FontTexture> textTexture;
	TextRenderPass TextRenderer;

public:
	GUIRenderer();
	GUIRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~GUIRenderer();

	static constexpr RendererID rendererID = RendererID::GUI_Renderer;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	void Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

