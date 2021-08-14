#pragma once
#include "Model.h"
#include "AssetManager.h"
#include "Tile.h"
#include "Sprite.h"
#include "LevelTile.h"
#include "AnimatedTileMesh.h"

class Level2D : public Model
{
private:
	std::shared_ptr<Timer> timer2;

protected:
	uint32_t TilesInTileSheet = 0;
	glm::ivec2 LevelBounds = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVSize = glm::vec2(1.0f, 1.0f);
	std::vector<uint32_t> TileLevelLayout;
	glm::vec3 LightDirection = glm::vec3(1.0f);
	std::shared_ptr<Material> material;

	void AddSprite(std::shared_ptr<Sprite> sprite);
	void AddTiles(std::shared_ptr<Material> material);
	void AddAnimatedTile(std::shared_ptr<Material> material, uint32_t TileIndex);
	void LoadLevel(std::shared_ptr<Material> material);
	std::vector<uint32_t> GetUniqueAnimatedTiles();
public:
	std::vector<std::shared_ptr<Tile>> TileList;
	std::vector<std::shared_ptr<LevelTile>> LevelTileLayout;
	Level2D();
	~Level2D();

	virtual void Update() override;
};

