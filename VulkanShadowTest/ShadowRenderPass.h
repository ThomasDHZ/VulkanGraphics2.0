#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "RenderedDepthTexture.h"
#include "ShadowRenderingPipeline.h"
#include "RenderedColorTexture.h"

class ShadowRenderPass
{
private:
	VkRenderPass RenderPass;

	void CreateRenderPass(VkDevice device);
	void CreateRendererFramebuffers(VkDevice device);

public:
	ShadowRenderPass();
	ShadowRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, VkExtent2D extent);
	~ShadowRenderPass();

	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedColorTexture> DebugColorTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<ShadowRenderingPipeline> shadowRendereringPipeline;
	std::shared_ptr<ShadowRenderingPipeline> AnimatedShadowRendereringPipeline;

	void UpdateSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent);
	void Destroy(VkDevice device);

	VkRenderPass GetRenderPass() { return RenderPass; }
};
