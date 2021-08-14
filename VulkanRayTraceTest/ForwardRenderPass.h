#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "AssetManager.h"
#include "Skybox.h"
#include "DebugLightRenderingPipeline.h"
#include "ForwardRenderingPipeline.h"
#include "SkyBoxRenderingPipeline.h"
#include "BaseRenderPass.h"
#include "PBRPipeline.h"
#include "RenderedColorTexture.h"

class ForwardRenderPass : public BaseRenderPass
{
private:

	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	ForwardRenderPass();
	~ForwardRenderPass();

	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DebugLightRenderingPipeline> DebugLightPipeline;
	std::shared_ptr<ForwardRenderingPipeline> forwardRenderingPipeline;
	std::shared_ptr<PBRPipeline> pbrRenderingPipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> skyBoxRenderingPipeline;

	void RebuildSwapChain();
	void Draw(RendererID rendererID);
	void Destroy() override;
};