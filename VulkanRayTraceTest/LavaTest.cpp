#include "LavaTest.h"
#include "MegaMan.h"

LavaTest::LavaTest() : Level2D()
{

}

LavaTest::LavaTest(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) : Level2D()
{
	std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
	material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/lava_diffuse.bmp", VK_FORMAT_R8G8B8A8_SRGB);
	material->materialTexture.SpecularMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/lava_Specular.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/Lava_normal.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/Lava_height.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.EmissionMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/lava_emission.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.AlphaMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/lava_Alpha.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	auto loadedmaterial = assetManager->materialManager.LoadMaterial(engine, "MegaManMaterial", material);

	TilesInTileSheet = 16;
	LevelBounds = glm::ivec2(16, 2);
	TileSize = glm::vec2(4.0f);
	TileUVSize = glm::vec2(.03125, 1.0f);
	
	std::vector<FrameOffset> LavaTopTiles = { glm::ivec2(0,0),
											  glm::ivec2(1,0),
											  glm::ivec2(2,0),
											  glm::ivec2(3,0),
											  glm::ivec2(4,0),
											  glm::ivec2(5,0),
											  glm::ivec2(6,0),
											  glm::ivec2(7,0) };
	std::vector<FrameOffset> LavaMiddleTiles = { glm::ivec2(8,0),
												 glm::ivec2(9,0),
												 glm::ivec2(10,0),
												 glm::ivec2(11,0),
												 glm::ivec2(12,0),
												 glm::ivec2(13,0),
												 glm::ivec2(14,0),
												 glm::ivec2(15,0) };
	std::shared_ptr<Animation2D> LavaTop = std::make_shared<Animation2D>(Animation2D(LavaTopTiles, 0.15f));
	std::shared_ptr<Animation2D> LavaMiddle = std::make_shared<Animation2D>(Animation2D(LavaMiddleTiles, 0.15f));

	TileList.emplace_back(std::make_shared<Tile>(Tile(0, glm::ivec2(0, 0), TileSize, TileUVSize, TilesInTileSheet, TileProperties::TileColliable, LavaTop)));
	TileList.emplace_back(std::make_shared<Tile>(Tile(1, glm::ivec2(15, 0), TileSize, TileUVSize, TilesInTileSheet, TileProperties::TileColliable, LavaMiddle)));

	TileLevelLayout = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	LightDirection = glm::vec3(0.12f, 0.035f, -0.279f);

	AddSprite(engine, assetManager, std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(2.0f, 7.0f, 0.0f))));
	LoadLevel(engine, assetManager, loadedmaterial);
}

LavaTest::~LavaTest()
{

}