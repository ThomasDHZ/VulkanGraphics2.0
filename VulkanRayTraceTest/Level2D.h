#pragma once
#include "Model.h"
#include "AssetManager.h"
#include "Tile.h"
#include "Sprite.h"

class Level2D : public Model
{
private:

protected:
	uint32_t TilesInTileSheet = 0;
	glm::ivec2 LevelBounds = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVSize = glm::vec2(1.0f, 1.0f);
	std::vector<uint32_t> TileLevelLayout;
	glm::vec3 LightDirection = glm::vec3(1.0f);
	std::shared_ptr<Material> material;

	void LoadLevel(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Material> material);
public:
	std::vector<std::shared_ptr<Tile>> TileList;
	std::vector<TileProperties> TilePropertiesList;
	Level2D();
	~Level2D();

	void AddSprite(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Sprite> sprite);
	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer) override;
};

