#pragma once
#include "BaseRenderer.h"
#include "FrameBufferRenderPass.h"
#include "PBRFrameBufferTextureRenderPass.h"
#include "CubeMapRenderPass.h"
#include "PrefilterRenderPass.h"
#include "BRDFRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;

	PBRFrameBufferTextureRenderPass FrameBufferTextureRenderer;
	//DepthDebugRenderPass DebugDepthRenderer;
	FrameBufferRenderPass FrameBufferRenderer;
	CubeMapRenderPass cubeMapRenderer;
	PrefilterRenderPass prefilterRenderPass;
	BRDFRenderPass brdfRenderPass;
public:
	PBRRenderer();
	PBRRenderer(std::shared_ptr<VulkanEngine> engine);
	~PBRRenderer();

	void RebuildSwapChain();
	void GUIUpdate();
	void Draw();
	void Destroy();

	 std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

