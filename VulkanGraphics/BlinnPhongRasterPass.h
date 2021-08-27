#pragma once
#include "BaseRenderPass.h"
#include "RenderedDepthTexture.h"
#include "BlinnPhongPipeline.h"
#include "MeshManager.h"
#include "RenderedColorTexture.h"

class BlinnPhongRasterPass : public BaseRenderPass
{
private:
	void CreateRenderPass();
	void CreateRendererFramebuffers();
	void SetUpCommandBuffers(std::shared_ptr<Mesh> mesh);

public:
	BlinnPhongRasterPass();
	BlinnPhongRasterPass(std::shared_ptr<Mesh> mesh, Texture2D texture, VkBuffer ubo);
	~BlinnPhongRasterPass();

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<RenderedColorTexture> BloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<BlinnPhongPipeline> blinnphongPipeline;

	void RebuildSwapChain(std::shared_ptr<Mesh> mesh, Texture2D texture, VkBuffer ubo);

	void Draw();
	void Destroy();
};

