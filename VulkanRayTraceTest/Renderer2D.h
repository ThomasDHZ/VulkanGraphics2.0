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
	Renderer2D(std::shared_ptr<VulkanEngine> engine);
	~Renderer2D();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

