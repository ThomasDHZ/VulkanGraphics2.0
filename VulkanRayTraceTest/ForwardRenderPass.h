#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "ForwardRenderingPipeline.h"

class ForwardRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);

public:
	ForwardRenderPass();
	ForwardRenderPass(VulkanEngine& engine);
	~ForwardRenderPass();

	VkRenderPass RenderPass;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<ForwardRenderingPipeline> forwardRendereringPipeline;

	void StartPipeline(VulkanEngine& engine, VkDescriptorSetLayout& DescriptorLayout);
	void UpdateSwapChain(VulkanEngine& engine, VkDescriptorSetLayout& DescriptorLayout);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};