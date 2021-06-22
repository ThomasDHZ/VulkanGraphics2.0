#pragma once
#include "AssetManager.h"
#include "Level2D.h"

class MetalManStage : public Level2D
{
private:

public:
	std::vector<uint32_t> TileLevelLayout;
	static constexpr uint32_t TilesInTileSheet = 30;
	static constexpr glm::ivec2 LevelBounds = glm::ivec2(16, 8);
	static constexpr glm::vec2 TileSize = glm::vec2(1.0f);
	static constexpr glm::vec2 TileUVSize = glm::vec2(.03333f, 1.0f);
	static constexpr glm::vec3 LightDirection = glm::vec3(0.12f, 0.035f, -0.279f);

	MetalManStage();
	MetalManStage(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position);
	~MetalManStage();
};

