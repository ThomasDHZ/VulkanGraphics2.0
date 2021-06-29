#include "SparkManStage.h"
SparkManStage::SparkManStage() : Level2D()
{

}

SparkManStage::SparkManStage(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) : Level2D()
{
	std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
	material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/SparkMan_diffuseOriginal.bmp", VK_FORMAT_R8G8B8A8_SRGB);
	material->materialTexture.SpecularMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/SparkMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/SparkMan_normal.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/SparkMan_height.bmp", VK_FORMAT_R8G8B8A8_UNORM);
	material->materialTexture.AlphaMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/SparkMan_Alpha.bmp", VK_FORMAT_R8G8B8A8_UNORM);

	TilesInTileSheet = 31;
	LevelBounds = glm::ivec2(16, 8);
	TileSize = glm::vec2(1.0f);
	TileUVSize = glm::vec2(.0238095238, 1.0f);
	TilePropertiesList.emplace_back(TileProperties{ 0, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), false, false });
	TilePropertiesList.emplace_back(TileProperties{ 1, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 2, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), false, true });
	TilePropertiesList.emplace_back(TileProperties{ 3, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 4, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 5, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 6, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 7, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 8, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 9, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 10, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 11, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 12, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 13, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 14, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 15, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 16, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 17, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 18, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 19, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 20, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 21, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 22, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 23, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 24, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 25, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 26, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 27, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 28, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 29, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });
	TilePropertiesList.emplace_back(TileProperties{ 30, glm::vec2(TilePropertiesList.size() * TileUVSize.x, TileUVSize.y), true, false });

	TileLevelLayout = { 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
						4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
						6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
						4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
						6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
						4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5,
					    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13 };
	LightDirection = glm::vec3(0.12f, 0.035f, -0.279f);

	LoadTiles(engine);

	MeshID = engine.GenerateID();
	MeshType = MeshTypeFlag::Mesh_Type_2D_Level;
	MeshMaterial = assetManager->materialManager.LoadMaterial(engine, "MegaManMaterial", material);

	MeshProperties = MeshPropertiesUniformBuffer(engine);
	ParentModelID = 0;

	MeshPosition = glm::vec3(0.0f);
	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, VertexList, IndexList);
}

SparkManStage::~SparkManStage()
{

}