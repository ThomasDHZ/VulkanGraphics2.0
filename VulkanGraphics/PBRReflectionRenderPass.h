#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "SkyBoxRenderPipeline.h"
#include "PBRPipeline.h"
#include "PBRReflectionPipeline.h"
#include "MultiViewSkyBoxPipeline.h"

class PBRReflectionRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

	std::shared_ptr<RenderedCubeMapTexture> ColorTexture;
	std::shared_ptr<RenderedCubeMapTexture> BloomTexture;

public:
	PBRReflectionRenderPass();
	PBRReflectionRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList);
	~PBRReflectionRenderPass();

	std::shared_ptr<RenderedCubeMapTexture> RenderedTexture;
	std::shared_ptr<RenderedCubeMapTexture> RenderedBloomTexture;

	std::shared_ptr<PBRReflectionPipeline> pbrPipeline;
	std::shared_ptr<MultiViewSkyBoxPipeline> skyboxPipeline;

	void RebuildSwapChain(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList);

	void Draw();
	void Destroy();
};

