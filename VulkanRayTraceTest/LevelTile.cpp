#include "LevelTile.h"

LevelTile::LevelTile()
{
}

LevelTile::LevelTile(std::vector<Vertex>& collisionVertexList, std::vector<uint32_t>& collisionIndexList, std::shared_ptr<Tile> tilez)
{
	tile = tilez;
	if (tilez->Collidable)
	{
		Collider = TileCollider(collisionVertexList, collisionIndexList);
	}
}

LevelTile::~LevelTile()
{
}
