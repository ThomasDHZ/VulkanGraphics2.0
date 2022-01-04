#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "PBRPipeline.h"
#include "PBRReflectionPipeline.h"
#include "MultiViewSkyboxPipeline.h"
#include "ReflectionViewCamera.h"

class PBRReflectionRenderPass : public BaseRenderPass
{
private:
	static constexpr RendererDrawFlag RendererType = RendererDrawFlag::Renderer_Draw_Reflection_Pass;

	std::shared_ptr<CubeSampler> cubeSampler;
	std::shared_ptr<ReflectionViewCamera> reflectionViewCamera;

	void CreateRenderPass();
	void CreateRendererFramebuffers();

public:
	glm::vec3 reflectPos = glm::vec3(0.0f);
	glm::vec2 XNearFar = glm::vec2(-3.0f, 3.0f);
	glm::vec2 YNearFar = glm::vec2(-3.0f, 3.0f);
	glm::vec2 ZNearFar = glm::vec2(-3.0f, 3.0f);

	PBRReflectionRenderPass();
	PBRReflectionRenderPass(glm::ivec2 renderPassResolution, glm::vec3 reflectViewPos, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);
	~PBRReflectionRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> RenderedBloomTexture;

	std::shared_ptr<PBRReflectionPipeline> pbrPipeline;
	std::shared_ptr<MultiViewSkyboxPipeline> skyboxPipeline;

	void RebuildSwapChain(glm::ivec2 renderPassResolution, glm::vec3 reflectViewPos, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList);

	void Draw();
	void Destroy();
};

