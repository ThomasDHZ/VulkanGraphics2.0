#include "Mario.h"

Mario::Mario()
{

}

Mario::Mario(std::shared_ptr<VulkanEngine> engine, AssetManager& assetManager, glm::vec3 Position) : Sprite(engine, SpriteSize, UVSize, Position, 0)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager.textureManager);
    material->materialTexture.DiffuseMap = assetManager.textureManager->LoadTexture2D("../texture/Mario_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.AlbedoMap = assetManager.textureManager->LoadTexture2D("../texture/Mario_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.NormalMap = assetManager.textureManager->LoadTexture2D("../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.SpecularMap = assetManager.textureManager->LoadTexture2D("../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AlphaMap = assetManager.textureManager->LoadTexture2D("../texture/Mario_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = assetManager.materialManager->LoadMaterial("MarioMaterial", material);

    std::vector<Frame2D> StandAnimation =
    {
        Frame2D(0.0f, 0.0f),
        Frame2D(0.1f, 0.0f),
        Frame2D(0.2f, 0.0f)
    };

    //std::vector<std::shared_ptr<Animation2D>> AnimationList
    //{
    //    std::make_shared<Animation2D>(FrameOffset(StandAnimation, 0.1f))
    //};

    //AnimationPlayer = AnimationPlayer2D(AnimationList, UVSize, SingleSpriteSize, SpritesInSpriteSheet, 0);
}

Mario::~Mario()
{

}

void Mario::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager)
{
}
