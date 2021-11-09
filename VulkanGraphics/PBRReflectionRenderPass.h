#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "PBRPipeline.h"
#include "PBRReflectionPipeline.h"
#include "MultiViewSkyboxPipeline.h"

class PBRReflectionRenderPass : public BaseRenderPass
{
private:
	std::shared_ptr<CubeSampler> cubeSampler;
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	glm::vec3 reflectPos = glm::vec3(-0.365314275f, 0.866172075f, 0.537497699f);
	glm::vec2 XNearFar = glm::vec2(-3.0f, 3.0f);
	glm::vec2 YNearFar = glm::vec2(-3.0f, 3.0f);
	glm::vec2 ZNearFar = glm::vec2(-3.0f, 3.0f);

	PBRReflectionRenderPass();
	PBRReflectionRenderPass(glm::ivec2 renderPassResolution, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList);
	~PBRReflectionRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> RenderedBloomTexture;

	std::shared_ptr<PBRReflectionPipeline> pbrPipeline;
	std::shared_ptr<MultiViewSkyboxPipeline> skyboxPipeline;

	void RebuildSwapChain(glm::ivec2 renderPassResolution, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList);

	void Draw();
	void Destroy();
};

