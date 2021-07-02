#include "LevelTile.h"

LevelTile::LevelTile()
{
}

LevelTile::LevelTile(std::shared_ptr<Tile> tileProperties, glm::vec3 position, std::vector<Vertex> collisionVertexList, std::vector<uint32_t> collisionIndexList)
{
	TileProperties = tileProperties;
	Position = position;
	CollisionVertexList = collisionVertexList;
	CollisionIndexList = collisionIndexList;

	if ((TileProperties->TilePropertiesFlags & TileProperties::TileColliable) != 0)
	{
		Collider = TileCollider(CollisionVertexList, CollisionIndexList);
	}
}

LevelTile::~LevelTile()
{
}
