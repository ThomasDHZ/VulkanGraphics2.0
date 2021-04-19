#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "AssetManager.h"
#include "Skybox.h"
#include "DebugLightRenderingPipeline.h"
#include "FowardRenderingPipeline.h"
#include "SkyBoxRenderingPipeline.h"
#include "WaterSurfacePipeline.h"
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

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DebugLightRenderingPipeline> DebugLightPipeline;
	std::shared_ptr<FowardRenderingPipeline> ForwardRenderingPipeline;
	std::shared_ptr<WaterSurfacePipeline> WaterSurfaceRenderingPipeline;

	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer, Skybox& skybox);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer);
	void Destroy(VulkanEngine& engine) override;
};