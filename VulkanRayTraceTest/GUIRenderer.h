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
	static constexpr RendererID rendererID = RendererID::GUI_Renderer;

	GUIRenderPass TextRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
public:
	GUIRenderer();
	GUIRenderer(std::shared_ptr<VulkanEngine> engine);
	~GUIRenderer();

	void RebuildSwapChain();
	void GUIUpdate();

	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};
