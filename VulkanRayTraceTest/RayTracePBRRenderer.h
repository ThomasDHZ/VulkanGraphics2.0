#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"
#include "SSAOBlurRenderPass.h"

class RayTracePBRRenderer : public BaseRenderer
{
private:
	FrameBufferRenderPass FrameBufferRenderer;
	SSAOBlurRenderPass SSAOBlurRenderer;
public:
	RayTracePBRRenderer();
	RayTracePBRRenderer(std::shared_ptr<VulkanEngine> engine);
	~RayTracePBRRenderer();

	static constexpr RendererID rendererID = RendererID::PBR_RayTrace_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};
