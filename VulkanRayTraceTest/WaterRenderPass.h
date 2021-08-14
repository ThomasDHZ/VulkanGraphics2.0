#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderWaterTexturePipeline.h"
#include "Skybox.h"
#include "BaseRenderPass.h"

class WaterRenderToTextureRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	WaterRenderToTextureRenderPass();
	WaterRenderToTextureRenderPass(std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer);
	~WaterRenderToTextureRenderPass();

	std::shared_ptr<PerspectiveCamera> TextureCamera;

	std::shared_ptr<SceneDataUniformBuffer> SceneDataBuffer;
	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	std::shared_ptr<RenderWaterTexturePipeline> WaterTexturePipeline;
	std::shared_ptr<SkyBoxRenderingPipeline> WaterSkyboxRenderingPipeline;


	void Update(SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera);
	void RebuildSwapChain(std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void Draw(Skybox skybox);
	void Destroy();
};
