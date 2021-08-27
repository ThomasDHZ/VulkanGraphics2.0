#pragma once
#include "BaseRenderer.h"
#include "BlinnPhongRasterPass.h"
#include "FrameBufferRenderPass.h"

class BlinnPhongRasterRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	BlinnPhongRasterPass BlinnRenderPass;
	FrameBufferRenderPass FrameBufferRenderer;

public:
	BlinnPhongRasterRenderer();
	BlinnPhongRasterRenderer(std::shared_ptr<Mesh> mesh, Texture2D texture, VkBuffer ubo);
	~BlinnPhongRasterRenderer();

	void RebuildSwapChain(std::shared_ptr<Mesh> mesh, Texture2D texture, VkBuffer ubo);
	void GUIUpdate();
	void Draw(int currentFrame);
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

