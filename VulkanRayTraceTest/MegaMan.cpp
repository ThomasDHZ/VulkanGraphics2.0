#include "MegaMan.h"

MegaMan::MegaMan()
{

}

MegaMan::MegaMan(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) : Sprite(engine, SpriteSize, UVSize, Position, 0)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.SpecularMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AlphaMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = assetManager->materialManager.LoadMaterial(engine, "MegaManMaterial", material);

    std::vector<Frame2D> StandAnimation =
    {
        Frame2D(0.0f, 0.0f),
        Frame2D(0.051867f, 0.0f),
    };

    std::vector<Frame2D> StandToRunTransitionAnimation =
    {
        Frame2D(0.103734f, 0.0f)
    };

    std::vector<Frame2D> RunAnimation =
    {
        Frame2D(0.18257f, 0.0f),
        Frame2D(0.23444f, 0.0f),
        Frame2D(0.27178f, 0.0f),
        Frame2D(0.23444f, 0.0f)
    };

    std::vector<Frame2D> SlideAnimation =
    {
        Frame2D(0.35062f, 0.0f)
    };

    std::vector<Frame2D> JumpAnimation =
    {
        Frame2D(0.42116f, 0.0f)
    };

    std::vector<Frame2D> ClimbAnimation =
    {
        Frame2D(0.48132f, 0.0f),
        Frame2D(0.51658f, 0.0f)
    };

    std::vector<Frame2D> ShootStandAnimation =
    {
        Frame2D(0.56016f, 0.0f)
    };

    std::vector<Frame2D> ShootRunAnimation =
    {
        Frame2D(0.62240f, 0.0f),
        Frame2D(0.69087f, 0.0f),
        Frame2D(0.74066f, 0.0f),
        Frame2D(0.69087f, 0.0f)
    };

    std::vector<Frame2D> ShootJumpAnimation =
    {
        Frame2D(0.89626f, 0.0f)
    };

    std::vector<Frame2D> ShootClimbAnimation =
    {
        Frame2D(0.94845f, 0.0f)
    };

    std::vector<std::shared_ptr<Animation2D>> AnimationList
    {
        std::make_shared<Animation2D>(Animation2D(StandAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(StandToRunTransitionAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(RunAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(SlideAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(JumpAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(ClimbAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(ShootStandAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(ShootRunAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(ShootJumpAnimation, 0.05f)),
        std::make_shared<Animation2D>(Animation2D(ShootClimbAnimation, 0.05f))
    };
    
    AnimationPlayer = AnimationPlayer2D(AnimationList);
}

MegaMan::~MegaMan()
{

}

void MegaMan::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer)
{
    if (inputManager.IsKeyPressed(KeyboardKey::KEY_LEFT) || 
        inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
    {
        AnimationPlayer.SetAnimation(kRunAnimation);
    }

    if (inputManager.IsKeyReleased(KeyboardKey::KEY_LEFT) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_RIGHT))
    {
        AnimationPlayer.SetAnimation(kStandAnimation);
    }

    Sprite::Update(engine, inputManager, materialManager, timer);
}
