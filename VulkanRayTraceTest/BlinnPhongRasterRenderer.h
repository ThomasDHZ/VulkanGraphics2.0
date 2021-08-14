#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "LightPathRenderer.h"

class BlinnPhongRasterRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;

	FrameBufferTextureRenderPass FrameBufferTextureRenderer;
	BloomRenderPass BloomRenderer;
	//DepthDebugRenderPass DebugDepthRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
	//DepthRenderer lightPathRenderer;
public:
	BlinnPhongRasterRenderer();
	BlinnPhongRasterRenderer(std::shared_ptr<AssetManager> assetManagerPtr);
	~BlinnPhongRasterRenderer();

	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

