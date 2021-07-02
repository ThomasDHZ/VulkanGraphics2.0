#pragma once
#include "TileCollider.h"
#include "Tile.h"

class LevelTile
{
private:
	std::vector<Vertex> CollisionVertexList;
	std::vector<uint32_t> CollisionIndexList;
public:
	glm::vec3 Position;
	std::shared_ptr<Tile> TileProperties;
	TileCollider Collider;

	LevelTile();
	LevelTile(std::shared_ptr<Tile> tileProperties, glm::vec3 position, std::vector<Vertex> collisionVertexList, std::vector<uint32_t> collisionIndexList);
	~LevelTile();
};