#pragma once
#include "TileCollider.h"
#include "Tile.h"
class LevelTile
{
private:
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;
public:
	glm::vec2 Position;
	glm::vec2 UVOffset;
	std::shared_ptr<Tile> tile;
	TileCollider Collider;

	LevelTile();
	LevelTile(std::vector<Vertex>& collisionVertexList, std::vector<uint32_t>& collisionIndexList, std::shared_ptr<Tile> tilez);
	~LevelTile();
};

