#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "SceneRenderingPipeline.h"
#include "RenderedColorTexture.h"
#include "SkyBoxPipeline.h"
#include "DebugLightRenderingPipeline.h"
#include "SceneRender2DPipeline.h"
#include "Model.h"
#include "SkyBoxMesh.h"
#include "LightManager.h"
#include "Object2D.h"

class SceneRenderPass
{
private:
	VkRenderPass RenderPass;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);

public:
	SceneRenderPass();
	SceneRenderPass(VulkanEngine& engine);
	~SceneRenderPass();

	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<SceneRenderingPipeline> sceneRenderingPipeline;
	std::shared_ptr<SceneRenderingPipeline> wireFrameRendereringPipeline;
	std::shared_ptr<SceneRenderingPipeline> AnimatedsceneRendereringPipeline;
	std::shared_ptr<SceneRenderingPipeline> AnimatedWireFramedRendereringPipeline;
	std::shared_ptr<SceneRender2DPipeline> sceneRendering2DPipeline;
	std::shared_ptr<SkyBoxPipeline> skyBoxPipeline;
	std::shared_ptr<DebugLightRenderingPipeline> debugLightRenderingPipeline;

	void FrameDebug();
	void Draw(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::vector<Mesh>& MeshList);
	void UpdateSwapChain(VulkanEngine& engine);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};