#pragma once
#include "VulkanEngine.h"
#include "TileCollider.h"

struct TileProperties
{
	uint32_t TileIndex;
	bool Collidable = true;
	bool Climable = false;
};

class Tile
{
private:
	uint32_t TileIndex;
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;

public:
	TileCollider Collider;
	bool Collidable = true;
	bool Climable = false;

	Tile();
	Tile(std::vector<Vertex>& vertexList, std::vector<uint32_t>& indexList, TileProperties& tileProperties);
	~Tile();
};