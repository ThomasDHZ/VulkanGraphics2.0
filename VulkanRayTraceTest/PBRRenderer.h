#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "CubeMapRenderPass.h"
#include "PrefilterRenderPass.h"
#include "BRDFRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "DepthDebugRenderPass.h"
#include "PBRFrameBufferTextureRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;

	PBRFrameBufferTextureRenderPass FrameBufferTextureRenderer;
	DepthDebugRenderPass DebugDepthRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
	CubeMapRenderPass cubeMapRenderer;
	PrefilterRenderPass prefilterRenderPass;
	BRDFRenderPass brdfRenderPass;
public:
	PBRRenderer();
	PBRRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr);
	~PBRRenderer();

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	void GUIUpdate(std::shared_ptr<VulkanEngine> engine);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	 std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

