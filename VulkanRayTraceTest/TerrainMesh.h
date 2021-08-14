#pragma once
#include "Mesh.h"
#include "HeightMapTexture.h"

class TerrainMesh : public Mesh
{
private:
	HeightMapTexture HeightMap;
	std::vector<uint32_t> IndexList;

	void BuildTerrainMesh(const std::string HeightMapLocation);
public:
	TerrainMesh();
	TerrainMesh(const std::string HeightMapLocation, std::shared_ptr<Material> material);
	~TerrainMesh();

	void Destory() override;
};

