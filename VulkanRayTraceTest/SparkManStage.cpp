#include "SparkManStage.h"
#include "MegaMan.h"
SparkManStage::SparkManStage() : Level2D()
{

}

SparkManStage::SparkManStage(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) : Level2D()
{
	std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
	material->materialTexture.DiffuseMap = assetManager->textureManager->LoadTexture2D("../texture/SparkMan_diffuseOriginal.bmp", VK_FORMAT_R8G8B8A8_SRGB);
	material->materialTexture.SpecularMap = assetManager->textureManager->LoadTexture2D("../texture/SparkMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.NormalMap = assetManager->textureManager->LoadTexture2D("../texture/SparkMan_normal.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.DepthMap = assetManager->textureManager->LoadTexture2D("../texture/SparkMan_height.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.EmissionMap = assetManager->textureManager->LoadTexture2D("../texture/SparkMan_Bloom.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.AlphaMap = assetManager->textureManager->LoadTexture2D("../texture/SparkMan_Alpha.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	auto loadedmaterial = assetManager->materialManager->LoadMaterial("MegaManMaterial", material);

	TilesInTileSheet = 42;
	LevelBounds = glm::ivec2(16, 8);
	TileSize = glm::vec2(1.0f);
	TileUVSize = glm::vec2(.0238095238, 1.0f);

	TileList.emplace_back(std::make_shared<Tile>(Tile(0, glm::ivec2(0, 0), TileSize, TileUVSize, TilesInTileSheet, TileNone)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(1, glm::ivec2(1, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(2, glm::ivec2(2, 0), TileSize, TileUVSize, TilesInTileSheet, TileClimable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(3, glm::ivec2(3, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(4, glm::ivec2(4, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(5, glm::ivec2(5, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(6, glm::ivec2(6, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(7, glm::ivec2(7, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(8, glm::ivec2(8, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(9, glm::ivec2(9, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(10, glm::ivec2(10, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(11, glm::ivec2(11, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(12, glm::ivec2(12, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(13, glm::ivec2(13, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(14, glm::ivec2(14, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(15, glm::ivec2(15, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(16, glm::ivec2(16, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(17, glm::ivec2(17, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(18, glm::ivec2(18, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(19, glm::ivec2(19, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(20, glm::ivec2(20, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(21, glm::ivec2(21, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(22, glm::ivec2(22, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(23, glm::ivec2(23, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(24, glm::ivec2(24, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(25, glm::ivec2(25, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(26, glm::ivec2(26, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(27, glm::ivec2(27, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(28, glm::ivec2(28, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(29, glm::ivec2(29, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(30, glm::ivec2(30, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(31, glm::ivec2(31, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(32, glm::ivec2(32, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(33, glm::ivec2(33, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(34, glm::ivec2(34, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(35, glm::ivec2(35, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(36, glm::ivec2(36, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(37, glm::ivec2(37, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(38, glm::ivec2(38, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(39, glm::ivec2(39, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(40, glm::ivec2(40, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(41, glm::ivec2(41, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(42, glm::ivec2(42, 0), TileSize, TileUVSize, TilesInTileSheet, TileColliable)));

	TileLevelLayout = { 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
						4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
						6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
						4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
						6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
						4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
					    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13 };
	LightDirection = glm::vec3(0.12f, 0.035f, -0.279f);

	AddSprite(engine, assetManager, std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(2.0f, 7.0f, 0.0f))));
	LoadLevel(engine, assetManager, loadedmaterial);
}

SparkManStage::~SparkManStage()
{

}