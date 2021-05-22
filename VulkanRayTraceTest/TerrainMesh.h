#pragma once
#include "Mesh.h"
#include "HeightMapTexture.h"

class TerrainMesh : public Mesh
{
private:
	HeightMapTexture HeightMap;
	std::vector<uint32_t> IndexList;

	void BuildTerrainMesh(VulkanEngine& engine, const std::string HeightMapLocation);
public:
	TerrainMesh();
	TerrainMesh(VulkanEngine& engine, const std::string HeightMapLocation);
	~TerrainMesh();

	void Destory(VulkanEngine& engine) override;
};

