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
class ForwardRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	ForwardRenderPass();
	ForwardRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~ForwardRenderPass();

	RenderPassID RendererID = Forward_Renderer;

	VkRenderPass RenderPass;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<DebugLightRenderingPipeline> DebugLightPipeline;
	std::shared_ptr<FowardRenderingPipeline> ForwardRenderingPipeline;
	std::shared_ptr<WaterSurfacePipeline> WaterSurfaceRenderingPipeline;

	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};