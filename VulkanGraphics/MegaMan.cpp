#include "MegaMan.h"

MegaMan::MegaMan() : Sprite()
{

}

MegaMan::MegaMan(glm::vec3 Position) : Sprite(SpriteSize, SpriteUVSize, Position, 0)
{
    MaterialTexture material{};
    material.DiffuseMap = AssetManagerPtr::GetAssetPtr()->textureManager->LoadTexture2D("../texture/MegaMan_diffuse.bmp", VK_FORMAT_R8G8B8A8_SRGB);
    material.AlbedoMap = AssetManagerPtr::GetAssetPtr()->textureManager->LoadTexture2D("../texture/MegaMan_diffuse.bmp", VK_FORMAT_R8G8B8A8_SRGB);
    material.NormalMap = AssetManagerPtr::GetAssetPtr()->textureManager->LoadTexture2D("../texture/MegaMan_normal.bmp", VK_FORMAT_R8G8B8A8_UNORM);
    material.SpecularMap = AssetManagerPtr::GetAssetPtr()->textureManager->LoadTexture2D("../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.AlphaMap = AssetManagerPtr::GetAssetPtr()->textureManager->LoadTexture2D("../texture/MegaMan_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = AssetManagerPtr::GetAssetPtr()->materialManager->LoadMaterial("MegaManMaterial", material);

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

void MegaMan::Update(float timer, std::vector<std::shared_ptr<LevelTile>> LevelTileLayout, std::vector<std::shared_ptr<Mesh>> MeshList)
{
   /* uint32_t newAnimation = 0;
    if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        newAnimation = kShootStandAnimation;
        megaManStatus = MegaManStatus::kShooting;
    }

    if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_LEFT) ||
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_RIGHT))
    {
        if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            Velocity.x = 0.01f;
            FlipSpriteX = false;
        }
        else
        {
            Velocity.x = -0.01f;
            FlipSpriteX = true;
        }
        newAnimation = kRunAnimation;
    }

    if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_UP) ||
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_DOWN))
    {
        if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_UP))
        {
            Velocity.y = 0.01f;
        }
        else
        {
            Velocity.y = -0.01f;
        }

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
    if ((InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_LEFT) ||
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_RIGHT)) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSpriteX = false;
        }
        else
        {
            FlipSpriteX = true;
        }
        newAnimation = kShootRunAnimation;
    }

    if ((InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_UP) ||
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_DOWN)) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_RIGHT))
        {
            FlipSpriteX = false;
        }
        else
        {
            FlipSpriteX = true;
        }
        newAnimation = kShootClimbAnimation;
    }

    if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_Z))
    {
        newAnimation = kJumpAnimation;
    }

    if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_Z) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_DOWN))
    {
        newAnimation = kSlideAnimation;
    }

    if (InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_Z) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyPressed(KeyboardKey::KEY_SPACE))
    {
        newAnimation = kShootJumpAnimation;
    }

    if (InputManagerPtr::GetInputManagerPtr()->IsKeyReleased(KeyboardKey::KEY_SPACE) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyReleased(KeyboardKey::KEY_Z) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyReleased(KeyboardKey::KEY_LEFT) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyReleased(KeyboardKey::KEY_RIGHT) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyReleased(KeyboardKey::KEY_UP) &&
        InputManagerPtr::GetInputManagerPtr()->IsKeyReleased(KeyboardKey::KEY_DOWN))
    {
        Velocity = glm::vec3(0.0f);
        newAnimation = kStandAnimation;
    }

    if (newAnimation != AnimationPlayer.GetCurrentAnimationIndex())
    {
        AnimationPlayer.SetAnimation(newAnimation);
    }
    Sprite::Update(LevelTileLayout, MeshList);*/
}
