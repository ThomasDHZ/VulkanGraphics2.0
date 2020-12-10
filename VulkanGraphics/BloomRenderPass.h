//#pragma once
//#include "SceneToTextureRenderer.h"
//#include "Mesh2D.h"
//#include "RenderedColorTexture.h"
//#include "BloomPipeline.h"
//#include "BloomPipeline2nd.h"
//
//class BloomRenderPass
//{
//private:
//	VkRenderPass RenderPass;
//	std::vector<VkFramebuffer> SwapChainFramebuffers;
//
//	std::shared_ptr<BloomPipeline> bloomPipeline;
//	std::shared_ptr<BloomPipeline2nd> bloomPipeline2nd;
//
//	std::shared_ptr<RenderedColorTexture> ColorTexture;
//	std::shared_ptr<RenderedDepthTexture> DepthTexture;
//
//	void CreateRenderPass(VulkanEngine& engine);
//	void CreateRendererFramebuffers(VulkanEngine& engine);
//
//public:
//	BloomRenderPass();
//	BloomRenderPass(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<GraphicsPipeline> Shader);
//	~BloomRenderPass();
//
//	void Render(VulkanEngine& engine);
//	void UpdateSwapChain(VulkanEngine& engine);
//	void Destroy(VulkanEngine& engine);
//
//	std::shared_ptr<RenderedColorTexture> GetColorTexture() { return ColorTexture; };
//};