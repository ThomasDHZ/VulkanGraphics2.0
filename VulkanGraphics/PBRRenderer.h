#pragma once
#include "BaseRenderer.h"
#include "PBRRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "IrradianceRenderPass.h"
#include "EquirectangularToCubemapRenderPass.h"
#include "PrefilterRenderPass.h"
#include "BRDFRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;
	int frameCount = 0;

	PBRRenderPass pbrRenderer;
	IrradianceRenderPass irradianceRenderPass;
	EquirectangularToCubemapRenderPass equirectangularToCubemapRenderPass;
	PrefilterRenderPass prefilterRenderPass;
	BRDFRenderPass brdfRenderPass;
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

