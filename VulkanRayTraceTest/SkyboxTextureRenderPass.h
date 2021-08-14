#pragma once
#include "Vulkanengine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "AssetManager.h"
#include "RenderedCubeMapTexture.h"
#include "PBRIrradiancePipeline.h"
#include "BaseRenderPass.h"

class SkyboxTextureRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();
	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;
public:
	SkyboxTextureRenderPass();
	SkyboxTextureRenderPass(std::shared_ptr<SceneDataUniformBuffer> sceneData, std::shared_ptr<PerspectiveCamera> camera);
	~SkyboxTextureRenderPass();

	SkyboxUniformBuffer sceneData;

	std::shared_ptr<PBRIrradiancePipeline> pbrIrradiancePipeline;
	std::shared_ptr<RenderedCubeMapTexture> CubeMapTexture;

	void UpdateSwapChain();
	void Draw();
	void Destroy();
};

