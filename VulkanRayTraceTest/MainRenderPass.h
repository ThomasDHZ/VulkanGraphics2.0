//#pragma once
//#include <vulkan/vulkan.h>
//#include <vector>
//#include <array>
//#include "VulkanEngine.h"
//#include "ForwardRenderingPipeline.h"
//
//enum RenderDrawFlags
//{
//	RenderNormally = 1 << 0,
//	RenderAnimated = 1 << 1,
//	RenderWireFrame = 1 << 2,
//	RenderWireFrameAnimated = 1 << 3,
//	RenderShadow = 1 << 4,
//	RenderShadowAnimated = 1 << 5,
//	RenderLightDebug = 1 << 6
//};
//
//class MainRenderPass
//{
//private:
//	VkRenderPass RenderPass;
//
//	void CreateRenderPass(VulkanEngine& engine);
//	void CreateRendererFramebuffers(VulkanEngine& engine);
//
//public:
//	MainRenderPass();
//	MainRenderPass(VulkanEngine& engine);
//	~MainRenderPass();
//
//	std::vector<VkFramebuffer> SwapChainFramebuffers;
//	std::shared_ptr<RenderedDepthTexture> DepthTexture;
//
//	std::shared_ptr<ForwardRenderingPipeline> wireFrameRendereringPipeline;
//	std::shared_ptr<ForwardRenderingPipeline> AnimatedForwardRendereringPipeline;
//	std::shared_ptr<ForwardRenderingPipeline> AnimatedWireFramedRendereringPipeline;
//
//	void UpdateSwapChain(VulkanEngine& engine);
//	void Destroy(VulkanEngine& engine);
//
//	VkRenderPass GetRenderPass() { return RenderPass; }
//};
