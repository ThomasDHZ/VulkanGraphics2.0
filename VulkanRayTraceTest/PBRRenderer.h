#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "CubeMapRenderPass.h"
#include "PrefilterRenderPass.h"
#include "BRDFRenderPass.h"
#include "FrameBufferTextureRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "DepthDebugRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;

	FrameBufferTextureRenderPass FrameBufferTextureRenderer;
	DepthDebugRenderPass DebugDepthRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
	CubeMapRenderPass cubeMapRenderer;
	PrefilterRenderPass prefilterRenderPass;
	BRDFRenderPass brdfRenderPass;
public:
	PBRRenderer();
	PBRRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~PBRRenderer();

	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine);

	 std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

