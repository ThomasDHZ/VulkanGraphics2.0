#include "MegaMan.h"

MegaMan::MegaMan()
{

}

MegaMan::MegaMan(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position) : Sprite(engine, SpriteSize, UVSize, Position, 0)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.SpecularMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AlphaMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MegaMan_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = assetManager->materialManager.LoadMaterial(engine, "MegaManMaterial", material);

    std::vector<Frame2D> StandAnimation =
    {
        Frame2D(0.0f, 0.0f),
        Frame2D(UVSize.x, 0.0f),
    };

    std::vector<Frame2D> StandToRunTransitionAnimation =
    {
        Frame2D(UVSize.x * 2, 0.0f)
    };

    std::vector<Frame2D> RunAnimation =
    {
        Frame2D(UVSize.x * 3, 0.0f),
        Frame2D(UVSize.x * 4, 0.0f),
        Frame2D(UVSize.x * 5, 0.0f),
        Frame2D(UVSize.x * 4, 0.0f)
    };

    std::vector<Frame2D> SlideAnimation =
    {
        Frame2D(UVSize.x * 6, 0.0f)
    };

    std::vector<Frame2D> JumpAnimation =
    {
        Frame2D(UVSize.x * 7, 0.0f)
    };

    std::vector<Frame2D> ClimbAnimation =
    {
        Frame2D(UVSize.x * 8, 0.0f),
        Frame2D(UVSize.x * 8, 0.0f)
    };

    std::vector<Frame2D> ClimbUpAnimation =
    {
        Frame2D(UVSize.x * 9, 0.0f)
    };

    std::vector<Frame2D> HurtAnimation =
    {
        Frame2D(UVSize.x * 10, 0.0f)
    };

    std::vector<Frame2D> ShootStandAnimation =
    {
        Frame2D(UVSize.x * 11, 0.0f)
    };

    std::vector<Frame2D> ShootRunAnimation =
    {
        Frame2D(UVSize.x * 12, 0.0f),
        Frame2D(UVSize.x * 13, 0.0f),
        Frame2D(UVSize.x * 14, 0.0f),
        Frame2D(UVSize.x * 13, 0.0f)
    };

    std::vector<Frame2D> ShootJumpAnimation =
    {
        Frame2D(UVSize.x * 15, 0.0f)
    };

    std::vector<Frame2D> ShootClimbAnimation =
    {
        Frame2D(UVSize.x * 16, 0.0f)
    };

    std::vector<Frame2D> LeftStandAnimation =
    {
        Frame2D(UVSize.x * (16 - 0), 0.0f),
        Frame2D(UVSize.x * (16 - 1), 0.0f),
    };

    std::vector<Frame2D> LeftStandToRunTransitionAnimation =
    {
        Frame2D(UVSize.x * (16 - 2), 0.0f)
    };

    std::vector<Frame2D> LeftRunAnimation =
    {
        Frame2D(UVSize.x * (16 - 3), 0.0f),
        Frame2D(UVSize.x * (16 - 4), 0.0f),
        Frame2D(UVSize.x * (16 - 5), 0.0f),
        Frame2D(UVSize.x * (16 - 4), 0.0f)
    };

    std::vector<Frame2D> LeftSlideAnimation =
    {
        Frame2D(UVSize.x * (16 - 6), 0.0f)
    };

    std::vector<Frame2D> LeftJumpAnimation =
    {
        Frame2D(UVSize.x* (16 - 7), 0.0f)
    };

    std::vector<Frame2D> LeftClimbAnimation =
    {
        Frame2D(UVSize.x * (16 - 8), 0.0f),
        Frame2D(UVSize.x * (16 - 8), 0.0f)
    };

    std::vector<Frame2D> LeftClimbUpAnimation =
    {
        Frame2D(UVSize.x* (16 - 9), 0.0f)
    };

    std::vector<Frame2D> LeftHurtAnimation =
    {
        Frame2D(UVSize.x* (16 - 10), 0.0f)
    };

    std::vector<Frame2D> LeftShootStandAnimation =
    {
        Frame2D(UVSize.x* (16 - 11), 0.0f)
    };

    std::vector<Frame2D> LeftShootRunAnimation =
    {
        Frame2D(UVSize.x * (16 - 12), 0.0f),
        Frame2D(UVSize.x * (16 - 13), 0.0f),
        Frame2D(UVSize.x * (16 - 14), 0.0f),
        Frame2D(UVSize.x * (16 - 13), 0.0f)
    };

    std::vector<Frame2D> LeftShootJumpAnimation =
    {
        Frame2D(UVSize.x * (16 - 15), 0.0f)
    };

    std::vector<Frame2D> LeftShootClimbAnimation =
    {
        Frame2D(UVSize.x* (16 - 16), 0.0f)
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
        std::make_shared<Animation2D>(Animation2D(LeftStandAnimation, 1.25f)),
        std::make_shared<Animation2D>(Animation2D(LeftStandToRunTransitionAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftRunAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftSlideAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftJumpAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftClimbAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftClimbUpAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftHurtAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftShootStandAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftShootRunAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftShootJumpAnimation, 0.15f)),
        std::make_shared<Animation2D>(Animation2D(LeftShootClimbAnimation, 0.15f))
    };
    
    AnimationPlayer = AnimationPlayer2D(AnimationList, SingleSpriteSize, SpritesInSpriteSheet, 0);
}

MegaMan::~MegaMan()
{

}

void MegaMan::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer)
{
    uint32_t newAnimation = 0;
    if (inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (FlipSprite)
        {
            newAnimation = kLeftShootStandAnimation;
        }
        else
        {
            newAnimation = kShootStandAnimation;
        }
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_LEFT) || 
        inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
    {
        if (inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSprite = false;
            newAnimation = kRunAnimation;
        }
        else
        {
            FlipSprite = true;
            newAnimation = kLeftRunAnimation;
        }
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_UP) ||
        inputManager.IsKeyPressed(KeyboardKey::KEY_DOWN))
    {
        if (AnimationPlayer.GetCurrentAnimationIndex() == kClimbAnimation)
        {
            if (AnimationPlayer.GetCurrentFrame() == 0)
            {
                FlipSprite = true;
            }
            else
            {
                FlipSprite = false;
            }
        }
        else
        {
            LastFlipSprite = FlipSprite;
        }
        newAnimation = kClimbAnimation;
    }

    if ((inputManager.IsKeyPressed(KeyboardKey::KEY_LEFT) ||
        inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT)) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSprite = false;
            newAnimation = kShootRunAnimation;
        }
        else
        {
            FlipSprite = true;
            newAnimation = kLeftShootRunAnimation;
        }
    }

    if ((inputManager.IsKeyPressed(KeyboardKey::KEY_UP) ||
        inputManager.IsKeyPressed(KeyboardKey::KEY_DOWN)) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (inputManager.IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSprite = false;
        }
        else
        {
            FlipSprite = true;
        }
        newAnimation = kShootClimbAnimation;
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_Z))
    {
        if (FlipSprite)
        {
            newAnimation = kLeftJumpAnimation;
        }
        else
        {
            newAnimation = kJumpAnimation;
        }
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_Z) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_DOWN))
    {
        if (FlipSprite)
        {
            newAnimation = kLeftSlideAnimation;
        }
        else
        {
            newAnimation = kSlideAnimation;
        }
    }

    if (inputManager.IsKeyPressed(KeyboardKey::KEY_Z) &&
        inputManager.IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (FlipSprite)
        {
            newAnimation = kLeftShootJumpAnimation;
        }
        else
        {
            newAnimation = kShootJumpAnimation;
        }
    }

    if (inputManager.IsKeyReleased(KeyboardKey::KEY_SPACE) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_Z) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_LEFT) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_RIGHT) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_UP) &&
        inputManager.IsKeyReleased(KeyboardKey::KEY_DOWN))
    {
        if (FlipSprite)
        {
            newAnimation = kLeftStandAnimation;
        }
        else
        {
            newAnimation = kStandAnimation;
        }
    }

    if (FlipSprite)
    {
       UVFlip.x = 1.0f;
    }
    else
    {
      UVFlip.x = 0.0f;
    }

    if (newAnimation != AnimationPlayer.GetCurrentAnimationIndex())
    {
        AnimationPlayer.SetAnimation(newAnimation);
    }
    Sprite::Update(engine, inputManager, materialManager, timer);
}
