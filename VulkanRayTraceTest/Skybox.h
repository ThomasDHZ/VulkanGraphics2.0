#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "SkyBoxRenderingPipeline.h"
#include "CubeMapRenderingPipeline.h"

class Skybox : public Mesh
{
private:
public:
	Skybox();
	Skybox(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass);
	~Skybox();

	void Update(VulkanEngine& engine, MaterialManager& materialManager, std::shared_ptr<PerspectiveCamera> camera, int imageView);
	void Destory(VulkanEngine& engine) override;
};

