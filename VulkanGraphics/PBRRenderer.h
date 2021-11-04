#pragma once
#include "BaseRenderer.h"
#include "PBRRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "IrradianceRenderPass.h"
#include "EquirectangularToCubemapRenderPass.h"
#include "PrefilterRenderPass.h"
#include "BRDFRenderPass.h"
#include "DepthPassRendererPass.h"
#include "DepthCubeMapRenderPass.h"
#include "DepthDebugRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;
	float CubeMapSamplerSize = 64.0f;

	DepthPassRendererPass DepthRenderPass;
	DepthCubeMapRenderPass depthCubeMapRenderPass;
	////DepthDebugRenderPass DebugDepthRenderPass;
	IrradianceRenderPass irradianceRenderPass;
	PrefilterRenderPass prefilterRenderPass;
	BRDFRenderPass brdfRenderPass;
	PBRRenderPass pbrRenderer;
	FrameBufferRenderPass FrameBufferRenderer;

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

