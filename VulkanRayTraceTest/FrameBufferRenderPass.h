#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"

class FrameBufferRenderPass
{
private:
	VkDescriptorSetLayout DescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void CreateDescriptorSetLayout(VulkanEngine& engine, AssetManager& assetManager);
	void CreateShaderPipeLine(VulkanEngine& engine);
	void CreateDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void CreateDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);

	void UpdateGraphicsPipeLine(VulkanEngine& engine);

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);
	~FrameBufferRenderPass();

	RenderPassID RendererID = FrameBuffer_Renderer;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	void Draw(VulkanEngine& engine, VkCommandBuffer commandbuffer, uint32_t index);
	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};