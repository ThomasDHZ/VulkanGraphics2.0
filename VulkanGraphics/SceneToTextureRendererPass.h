#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "ForwardRenderingPipeline.h"
#include "RenderedColorTexture.h"
#include "ForwardRenderering2DPipeline.h"
#include "Object2D.h"

class SceneToTextureRendererPass
{
private:
	VkRenderPass RenderPass;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);

public:
	SceneToTextureRendererPass();
	SceneToTextureRendererPass(VulkanEngine& engine);
	~SceneToTextureRendererPass();

	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<ForwardRenderingPipeline> forwardRendereringPipeline;
	std::shared_ptr<ForwardRenderering2DPipeline> forwardRenderering2DPipeline;
	
	void Draw(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void UpdateSwapChain(VulkanEngine& engine);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};
