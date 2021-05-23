#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"
#include "HybridFrameBufferRenderPass.h"
#include "DeferredRenderPass.h"
#include "SSAORenderPass.h"
#include "SSAOBlurRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"

class HybridRenderer : public BaseRenderer
{
private:
	int CurrentSSAOSampleRate;
	GBufferRenderPass FrameBufferTextureRenderer;
	HybridFrameBufferRenderPass FrameBufferRenderer;
	//DepthDebugRenderPass DebugDepthRenderer;
	SSAORenderPass SSAORenderer;
	SSAOBlurRenderPass SSAOBlurRenderer;
	BloomRenderPass bloomRenderPass;

	bool ApplySSAO = true;
public:
	HybridRenderer();
	HybridRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~HybridRenderer();

	static constexpr RendererID rendererID = RendererID::Hybrid_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

