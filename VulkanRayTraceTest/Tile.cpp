#include "Tile.h"

Tile::Tile()
{
}

Tile::Tile(uint32_t tileIndex, glm::ivec2 tileOffset, glm::vec2& tileSize, glm::vec2& tileUVSize, uint32_t tilesInSet, int tilePropertiesFlags, std::shared_ptr<Animation2D> TileAnimation)
{
	TileIndex = tileIndex;
	TileOffset = tileOffset;
	TileSize = tileSize;
	TilesInSet = tilesInSet;
	TileUVSize = tileUVSize;
	TilePropertiesFlags = tilePropertiesFlags;

	if (TileAnimation != nullptr)
	{
		AnimationPlayer = AnimationPlayer2D(TileAnimation, TileUVSize, TilesInSet);
	}
}

Tile::~Tile()
{
}
