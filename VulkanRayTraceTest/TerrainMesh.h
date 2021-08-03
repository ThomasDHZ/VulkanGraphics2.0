#pragma once
#include "Mesh.h"
#include "HeightMapTexture.h"

class TerrainMesh : public Mesh
{
private:
	HeightMapTexture HeightMap;
	std::vector<uint32_t> IndexList;

	void BuildTerrainMesh(std::shared_ptr<VulkanEngine> engine, const std::string HeightMapLocation);
public:
	TerrainMesh();
	TerrainMesh(std::shared_ptr<VulkanEngine> engine, const std::string HeightMapLocation, std::shared_ptr<Material> material);
	~TerrainMesh();

	void Destory(std::shared_ptr<VulkanEngine> engine) override;
};

