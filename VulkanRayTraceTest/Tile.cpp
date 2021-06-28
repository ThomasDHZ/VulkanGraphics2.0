#include "Tile.h"

Tile::Tile()
{
}

Tile::Tile(std::vector<Vertex>& vertexList, std::vector<uint32_t>& indexList, TileProperties& tileProperties)
{
	TileIndex = tileProperties.TileIndex;
	VertexList = vertexList;
	IndexList = indexList;
	Collidable = tileProperties.Collidable;
	Climable = tileProperties.Climable;

	if (Collidable)
	{
		Collider = TileCollider(vertexList, indexList);
	}
}

Tile::~Tile()
{
}
