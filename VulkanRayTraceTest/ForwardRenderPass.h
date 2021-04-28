#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
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

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	ForwardRenderPass();
	ForwardRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer);
	~ForwardRenderPass();

	static constexpr RenderPassID RendererID = Forward_Renderer;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DebugLightRenderingPipeline> DebugLightPipeline;
	std::shared_ptr<ForwardRenderingPipeline> forwardRenderingPipeline;
	std::shared_ptr<PBRPipeline> pbrRenderingPipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> skyBoxRenderingPipeline;

	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, Skybox& skybox);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex);
	void Destroy(VulkanEngine& engine) override;
};