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

	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);
	void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

public:
	ForwardRenderPass();
	ForwardRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	~ForwardRenderPass();

	static constexpr RenderPassID rendererPassID = Forward_Renderer;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DebugLightRenderingPipeline> DebugLightPipeline;
	std::shared_ptr<ForwardRenderingPipeline> forwardRenderingPipeline;
	std::shared_ptr<PBRPipeline> pbrRenderingPipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> skyBoxRenderingPipeline;

	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, RendererID rendererID);
	void Destroy(std::shared_ptr<VulkanEngine> engine) override;
};