#include "Tile.h"

Tile::Tile()
{
}

Tile::Tile(uint32_t tileIndex, glm::ivec2 tileOffset, glm::vec2& tileSize, glm::vec2& tileUVOffset, uint32_t tilesInSet, int tilePropertiesFlags, std::shared_ptr<Animation2D> TileAnimation)
{
	TileIndex = tileIndex;
	TileOffset = tileOffset;
	TileSize = tileSize;
	TilesInSet = tilesInSet;
	TileUVOffset = tileUVOffset;
	TilePropertiesFlags = tilePropertiesFlags;

	if (TileAnimation != nullptr)
	{
		AnimationPlayer = AnimationPlayer2D(TileAnimation, TileUVOffset, TilesInSet);
	}
}

Tile::~Tile()
{
}
