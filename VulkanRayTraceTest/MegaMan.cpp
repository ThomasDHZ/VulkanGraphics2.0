#include "MegaMan.h"

MegaMan::MegaMan()
{

}

MegaMan::MegaMan(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) : Sprite(engine, SpriteSize, SpriteUVSize, Position, 0)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.SpecularMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AlphaMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = assetManager->materialManager.LoadMaterial(engine, "MegaManMaterial", material);

    std::vector<FrameOffset> StandAnimation =
    {
        FrameOffset(0, 0),
        FrameOffset(1, 0),
    };

    std::vector<FrameOffset> StandToRunTransitionAnimation =
    {
        FrameOffset(2, 0)
    };

    std::vector<FrameOffset> RunAnimation =
    {
        FrameOffset(3, 0),
        FrameOffset(4, 0),
        FrameOffset(5, 0),
        FrameOffset(4, 0)
    };

    std::vector<FrameOffset> SlideAnimation =
    {
        FrameOffset(6, 0)
    };

    std::vector<FrameOffset> JumpAnimation =
    {
        FrameOffset(7, 0)
    };

    std::vector<FrameOffset> ClimbAnimation =
    {
        FrameOffset(8, 0),
        FrameOffset(8, 0)
    };

    std::vector<FrameOffset> ClimbUpAnimation =
    {
        FrameOffset(9, 0)
    };

    std::vector<FrameOffset> HurtAnimation =
    {
        FrameOffset(10, 0)
    };

    std::vector<FrameOffset> ShootStandAnimation =
    {
        FrameOffset(11, 0)
    };

    std::vector<FrameOffset> ShootRunAnimation =
    {
        FrameOffset(12, 0),
        FrameOffset(13, 0),
        FrameOffset(14, 0),
        FrameOffset(13, 0)
    };

    std::vector<FrameOffset> ShootJumpAnimation =
    {
        FrameOffset(15, 0)
    };

    std::vector<FrameOffset> ShootClimbAnimation =
    {
        FrameOffset(16, 0)
    };

    std::vector<std::shared_ptr<Animation2D>> AnimationList
    {
        std::make_shared<Animation2D>(Animation2D(StandAnimation, 1.25f)),
        std::make_shared<Animation2D>(Animation2D(StandToRunTransitionAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(RunAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(SlideAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(JumpAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(ClimbAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(ClimbUpAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(HurtAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(ShootStandAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(ShootRunAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(ShootJumpAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(ShootClimbAnimation, 0.15f)),
    };
    
    AnimationPlayer = AnimationPlayer2D(AnimationList, SpriteUVSize, SpritesInSpriteSheet, kStandAnimation);
}

MegaMan::~MegaMan()
{

}

void MegaMan::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer)
{
    uint32_t newAnimation = 0;
    if (inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        newAnimation = kShootStandAnimation;
        megaManStatus = MegaManStatus::kShooting;
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_LEFT) || 
        inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
    {
        if (inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSpriteX = false;
        }
        else
        {
            FlipSpriteX = true;
        }
        newAnimation = kRunAnimation;
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_UP) ||
        inputManager.IsKeyPressed(KeyboardKey::KEY_DOWN))
    {
        if (AnimationPlayer.GetCurrentAnimationIndex() == kClimbAnimation)
        {
            if (AnimationPlayer.GetCurrentFrame() == 0)
            {
                FlipSpriteX = true;
            }
            else
            {
                FlipSpriteX = false;
            }
        }
        else
        {
            LastFlipSpriteX = FlipSpriteX;
        }
        newAnimation = kClimbAnimation;
        megaManStatus = MegaManStatus::kClimbing;
    }

    if ((megaManStatus & MegaManStatus::kClimbing) != 0)
    {
        int a = 34;
    }
    if ((inputManager.IsKeyPressed(KeyboardKey::KEY_LEFT) ||
        inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT)) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSpriteX = false;
        }
        else
        {
            FlipSpriteX = true;
        }
        newAnimation = kShootRunAnimation;
    }

    if ((inputManager.IsKeyPressed(KeyboardKey::KEY_UP) ||
        inputManager.IsKeyPressed(KeyboardKey::KEY_DOWN)) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSpriteX = false;
        }
        else
        {
            FlipSpriteX = true;
        }
        newAnimation = kShootClimbAnimation;
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_Z))
    {
        newAnimation = kJumpAnimation;
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_Z) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_DOWN))
    {
        newAnimation = kSlideAnimation;
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_Z) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        newAnimation = kShootJumpAnimation;
    }

    if (inputManager.IsKeyReleased(KeyboardKey::KEY_SPACE) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_Z) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_LEFT) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_RIGHT) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_UP) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_DOWN))
    {
        newAnimation = kStandAnimation;
    }

    if (newAnimation != AnimationPlayer.GetCurrentAnimationIndex())
    {
        AnimationPlayer.SetAnimation(newAnimation);
    }
    Sprite::Update(engine, inputManager, materialManager, timer);
}
