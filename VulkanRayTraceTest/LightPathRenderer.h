#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "BloomRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "DepthRenderer.h"

class LightPathRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::LightPath_Renderer;

	DepthRenderer lightRenderPass;
	DepthDebugRenderPass DebugDepthRenderer;
public:
	LightPathRenderer();
	LightPathRenderer(std::shared_ptr<VulkanEngine> engine);
	~LightPathRenderer();

	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

