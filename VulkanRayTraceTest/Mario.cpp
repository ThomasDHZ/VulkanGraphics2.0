#include "Mario.h"

Mario::Mario()
{

}

Mario::Mario(glm::vec3 Position) : Sprite(SpriteSize, UVSize, Position, 0)
{
    std::shared_ptr<Material> material = std::make_shared<Material>(Material(EnginePtr::GetEnginePtr()));
    material->materialTexture.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("../texture/Mario_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("../texture/Mario_diffuse.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.SpecularMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AlphaMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("../texture/Mario_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    MeshMaterial = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("MarioMaterial", material);

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

void Mario::Update()
{
}
