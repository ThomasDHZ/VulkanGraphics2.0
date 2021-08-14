#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"
#include "HybridFrameBufferRenderPass.h"
#include "GBufferRenderPass.h"
#include "SSAORenderPass.h"
#include "SSAOBlurRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "GBufferRenderPass2.h"

class HybridRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::Hybrid_Renderer;

	int CurrentSSAOSampleRate;
	GBufferRenderPass FrameBufferTextureRenderer;
	GBufferRenderPass2 FrameBufferTextureRenderer2;
	HybridFrameBufferRenderPass FrameBufferRenderer;
	//DepthDebugRenderPass DebugDepthRenderer;
	SSAORenderPass SSAORenderer;
	SSAOBlurRenderPass SSAOBlurRenderer;
	BloomRenderPass bloomRenderPass;

	bool ApplySSAO = true;
public:
	HybridRenderer();
	HybridRenderer(std::shared_ptr<VulkanEngine> engine);
	~HybridRenderer();

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

