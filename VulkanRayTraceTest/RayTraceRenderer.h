#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"

class RayTraceRenderer : public BaseRenderer
{
private:

	FrameBufferRenderPass FrameBufferRenderer;
public:
	RayTraceRenderer();
	RayTraceRenderer(std::shared_ptr<VulkanEngine> engine);
	~RayTraceRenderer();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_RayTrace_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

