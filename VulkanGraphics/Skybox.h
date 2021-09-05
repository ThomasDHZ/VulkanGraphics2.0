#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"


class Skybox : public Mesh
{
private:
public:
	Skybox();
	Skybox(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
	~Skybox();
};

