#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "ForwardRenderingPipeline.h"
#include "RenderedDepthTexture.h"

enum RenderDrawFlags
{
	RenderNormally = 1 << 0,
	RenderAnimated = 1 << 1,
	RenderWireFrame = 1 << 2,
	RenderWireFrameAnimated = 1 << 3,
	RenderShadow = 1 << 4,
	RenderShadowAnimated = 1 << 5,
	RenderLightDebug = 1 << 6
};

class MainRenderPass
{
private:


	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);

public:
	MainRenderPass();
	MainRenderPass(VulkanEngine& engine);
	~MainRenderPass();

	VkRenderPass RenderPass;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<ForwardRenderingPipeline> pipeline;

	void UpdateSwapChain(VulkanEngine& engine);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};
