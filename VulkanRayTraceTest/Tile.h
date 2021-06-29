#pragma once
#include "VulkanEngine.h"
#include "TileCollider.h"

struct TileProperties
{
	uint32_t TileIndex;
	glm::vec2 TileOffset = glm::vec2(0.0f);
	bool Collidable = true;
	bool Climable = false;
};

class Tile
{
private:
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;

public:
	uint32_t TileIndex;
	glm::vec2 TileOffset = glm::vec2(0.0f);
	TileCollider Collider;
	bool Collidable = true;
	bool Climable = false;

	Tile();
	Tile(std::vector<Vertex>& vertexList, std::vector<uint32_t>& indexList, TileProperties& tileProperties);
	~Tile();
};