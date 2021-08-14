#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"

class RayTraceRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::BlinnPhong_RayTrace_Renderer;

	FrameBufferRenderPass FrameBufferRenderer;
public:
	RayTraceRenderer();
	RayTraceRenderer(std::shared_ptr<VulkanEngine> engine);
	~RayTraceRenderer();

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

