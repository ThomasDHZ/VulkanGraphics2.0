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
#include "PBRReflectionRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;
	float CubeMapSamplerSize = 512.0f;
	int selected = 0;

	BRDFRenderPass brdfRenderPass;

	DepthPassRendererPass DepthRenderPass;
	DepthCubeMapRenderPass depthCubeMapRenderPass;
	DepthPassRendererPass SpotLightDepthRenderPass;

	IrradianceRenderPass ReflectionIrradianceRenderPass;
	PrefilterRenderPass ReflectionPrefilterRenderPass;
	PBRReflectionRenderPass reflectionPBRPass;

	IrradianceRenderPass irradianceRenderPass;
	PrefilterRenderPass prefilterRenderPass;

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

