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
	RayTracePBRRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~RayTracePBRRenderer();

	static constexpr RendererID rendererID = RendererID::PBR_RayTrace_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};
