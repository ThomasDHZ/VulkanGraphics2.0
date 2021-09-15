#pragma once
#include "BaseRenderer.h"
#include "FrameBufferRenderPass.h"
#include "RenderPass2D.h"

class Renderer2D : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	RenderPass2D rendererPass2D;
	FrameBufferRenderPass FrameBufferRenderer;
public:
	Renderer2D();
	Renderer2D(std::shared_ptr<VulkanEngine> engine);
	~Renderer2D();

	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

