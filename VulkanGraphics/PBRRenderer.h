#pragma once
#include "BaseRenderer.h"
#include "PBRRenderPass.h"
#include "FrameBufferRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;

	PBRRenderPass pbrRenderer;
	FrameBufferRenderPass FrameBufferRenderer;

public:
	PBRRenderer();
	PBRRenderer(std::shared_ptr<VulkanEngine> engine);
	~PBRRenderer();

	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

