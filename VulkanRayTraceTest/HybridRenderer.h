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
	HybridRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~HybridRenderer();

	static constexpr RendererID rendererID = RendererID::Hybrid_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

