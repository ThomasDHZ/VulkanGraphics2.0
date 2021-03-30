#include "Mario.h"

Mario::Mario()
{

}

Mario::Mario(VulkanEngine& engine, AssetManager& assetManager, glm::vec3 Position) : Sprite(engine, SpriteSize, UVSize, Position, 0)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager.textureManager);
    material->materialTexture.DiffuseMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/Mario_diffuse.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.NormalMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.SpecularMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AlphaMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/Mario_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MaterialID = assetManager.materialManager.LoadMaterial(engine, "MarioMaterial", material);
}

Mario::~Mario()
{

}