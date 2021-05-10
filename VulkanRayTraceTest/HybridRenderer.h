#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "HybridFrameBufferRenderPass.h"
#include "RayTraceRenderPass.h"

class HybridRenderer : public BaseRenderer
{
private:

	HybridFrameBufferRenderPass FrameBufferRenderer;
	FrameBufferTextureRenderPass FrameBufferTextureRenderer;
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

