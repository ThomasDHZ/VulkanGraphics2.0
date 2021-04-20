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

class ForwardRenderPass : public BaseRenderPass
{
private:

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	ForwardRenderPass();
	ForwardRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~ForwardRenderPass();

	static constexpr RenderPassID RendererID = Forward_Renderer;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DebugLightRenderingPipeline> DebugLightPipeline;
	std::shared_ptr<ForwardRenderingPipeline> forwardRenderingPipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> SkyBoxRenderingPipeline;

	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer, Skybox& skybox);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer);
	void Destroy(VulkanEngine& engine) override;
};