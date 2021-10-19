#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"
#include "RenderedCubeMapTexture.h"
#include "BRDFPipeline.h"

class BRDFRenderPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers();

public:
	BRDFRenderPass();
	BRDFRenderPass(uint32_t textureSize);
	~BRDFRenderPass();

	std::shared_ptr<RenderedColorTexture> BRDFMap;
	std::shared_ptr<BRDFPipeline> brdfPipeline;

	void RebuildSwapChain(uint32_t textureSize);

	void Draw();
	void Destroy();
};

