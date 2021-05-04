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
	Skybox(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
	~Skybox();
};

