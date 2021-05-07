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
	RayTraceRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~RayTraceRenderer();

	static constexpr RendererID rendererID = RendererID::BlinnPhong_RayTrace_Renderer;

	RayTraceRenderPass rayTraceRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

