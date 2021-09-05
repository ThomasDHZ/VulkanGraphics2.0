//#pragma once
//#include <vulkan/vulkan.h>
//#include <vector>
//#include <array>
//#include "VulkanEngine.h"
//#include "RenderedDepthTexture.h"
//#include "FrameBufferRenderingPipeline.h"
//#include "Vertex.h"
//#include "Mesh.h"
//
//class FrameBufferRenderPass
//{
//private:
//
//	std::vector<Vertex> FrameBufferVertices =
//	{
//		{{1.0f, 1.0f, 0.0f},   {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
//		{{1.0f, -1.0f, 0.0f},  {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
//		{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
//		{{-1.0f, 1.0f, 0.0f},  {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
//	};
//
//	std::vector<uint32_t> FrameBufferIndices = {
//	   0, 1, 3, 1, 2, 3
//	};
//
//	Mesh mesh;
//	VkDescriptorSetLayout descriptorLayout;
//	VkDescriptorPool descriptorPool;
//	std::vector<VkDescriptorSet> descriptorSets;
//	VkRenderPass RenderPass;
//
//	void CreateRenderPass(VulkanEngine& engine);
//	void CreateRendererFramebuffers(VulkanEngine& engine);
//	void CreateDescriptorSetLayout(VulkanEngine& engine);
//	void CreateDescriptorPool(VulkanEngine& engine);
//	void CreateDescriptorSets(VulkanEngine& engine, std::shared_ptr<Texture> texture);
//
//public:
//	FrameBufferRenderPass();
//	FrameBufferRenderPass(VulkanEngine& engine, std::shared_ptr<Texture> texture);
//	~FrameBufferRenderPass();
//
//	std::vector<VkFramebuffer> SwapChainFramebuffers;
//
//	std::shared_ptr<FrameBufferRenderingPipeline> frameBufferPipeline;
//
//	void Draw(VulkanEngine& engine, VkCommandBuffer commandbuffer, uint32_t index);
//	void UpdateSwapChain(VulkanEngine& engine);
//	void Destroy(VulkanEngine& engine);
//
//	VkRenderPass GetRenderPass() { return RenderPass; }
//};