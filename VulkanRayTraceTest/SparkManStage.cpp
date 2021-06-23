#include "SparkManStage.h"
SparkManStage::SparkManStage() : Level2D()
{

}

SparkManStage::SparkManStage(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) :
    Level2D(engine, TilesInTileSheet, LevelBounds, TileSize, TileUVSize, { 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
                                                                           3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4,
                                                                           5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
                                                                           3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4,
                                                                           5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
                                                                           3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4,
                                                                           13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 5 }, LightDirection)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MetalMan_diffuseOriginal.bmp", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.SpecularMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/SpecularMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MetalMan_normal.bmp", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MetalMan_height.bmp", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = assetManager->materialManager.LoadMaterial(engine, "MegaManMaterial", material);
}

SparkManStage::~SparkManStage()
{

}