#pragma once
#include "BaseRenderer.h"
#include "BlinnPhongRasterPass.h"
#include "FrameBufferRenderPass.h"
#include "DepthPassRendererPass.h"
#include "DepthDebugRenderPass.h"
#include "DepthCubeMapRenderPass.h"
#include "ReflectionCubeMapRenderPass.h"

class BlinnPhongRasterRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::BlinnPhong_Raster_Renderer;
	bool cubeMapCheck = false;
	DepthPassRendererPass DepthRenderPass;
	DepthCubeMapRenderPass depthCubeMapRenderPass;
	//DepthDebugRenderPass DebugDepthRenderPass;
	//ReflectionCubeMapRenderPass ReflectionRenderPass;
	BlinnPhongRasterPass BlinnRenderPass;
	FrameBufferRenderPass FrameBufferRenderer;

public:
	BlinnPhongRasterRenderer();
	BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine);
	~BlinnPhongRasterRenderer();

	void RebuildSwapChain();
	void GUIUpdate();
	void Update();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

