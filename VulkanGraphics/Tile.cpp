#include "Tile.h"

Tile::Tile()
{
}

Tile::Tile(uint32_t tileIndex, glm::ivec2 tileOffset, glm::vec2& tileSize, glm::vec2& tileUVSize, uint32_t tilesInSet, int tilePropertiesFlags)
{
	TileIndex = tileIndex;
	TileOffset = tileOffset;
	TileSize = tileSize;
	TilesInSet = tilesInSet;
	TileUVSize = tileUVSize;
	TilePropertiesFlags = tilePropertiesFlags;
	LevelUVPosition = glm::vec2((float)TileOffset.x / (float)TilesInSet, (float)TileOffset.y / (float)TilesInSet);
}

Tile::Tile(uint32_t tileIndex, std::shared_ptr<Animation2D> TileAnimation, glm::vec2& tileSize, glm::vec2& tileUVSize, uint32_t tilesInSet, int tilePropertiesFlags)
{
	TileIndex = tileIndex;
	TileOffset = TileAnimation->GetFrame();
	TileSize = tileSize;
	TilesInSet = tilesInSet;
	TileUVSize = tileUVSize;
	TilePropertiesFlags = tilePropertiesFlags;
	LevelUVPosition = glm::vec2((float)TileOffset.x / (float)TilesInSet, (float)TileOffset.y / (float)TilesInSet);

	if (TileAnimation != nullptr)
	{
		AnimationPlayer = AnimationPlayer2D(TileAnimation, TileUVSize, TilesInSet);
	}
}

Tile::~Tile()
{
}
