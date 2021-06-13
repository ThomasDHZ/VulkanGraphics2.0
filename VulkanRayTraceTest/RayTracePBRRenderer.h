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
	RayTracePBRRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~RayTracePBRRenderer();

	static constexpr RendererID rendererID = RendererID::PBR_RayTrace_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};
