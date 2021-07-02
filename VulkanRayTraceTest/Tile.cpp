#include "Tile.h"

Tile::Tile()
{
}

Tile::Tile(uint32_t tileIndex, glm::ivec2 tileOffset, glm::vec2& tileSize, uint32_t tilesInSet, int tilePropertiesFlags, std::shared_ptr<Animation2D> TileAnimation)
{
	TileIndex = tileIndex;
	TileOffset = tileOffset;
	TileSize = tileSize;
	TilesInSet = tilesInSet;
	TileUVOffset = glm::vec2((float)TileOffset.x / (float)TilesInSet, (float)TileOffset.y / (float)TilesInSet);
	TilePropertiesFlags = tilePropertiesFlags;

	if (TileAnimation != nullptr)
	{
		AnimationPlayer.AddAnimation(TileAnimation);
	}
}

Tile::~Tile()
{
}
