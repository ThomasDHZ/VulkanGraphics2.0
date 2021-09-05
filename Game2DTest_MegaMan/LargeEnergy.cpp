//#include "LargeEnergy.h"
//
//#include "MegaMan.h"
//#include <algorithm>
//#include <functional>
//
//LargeEnergy::LargeEnergy() : Sprite()
//{
//}
//
//LargeEnergy::LargeEnergy(RendererManager& renderer, std::shared_ptr<TextureManager>textureManager, glm::vec2 StartPos) : Sprite()
//{
//	const std::vector<Vertex> SpriteVertices =
//	{
//		{{0.0f, 0.0f, -0.1f},				 {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
//		{{SpriteSize.x, 0.0f, -0.1f},		 {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
//		{{SpriteSize.x, SpriteSize.y, -0.1f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
//		{{0.0f, SpriteSize.y, -0.1f},		 {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
//	};
//
//	std::vector<Frame2D> LargeEnergyFlickerFrames;
//	LargeEnergyFlickerFrames.emplace_back(Frame2D(glm::vec2(0.0f, 0.0f)));
//	LargeEnergyFlickerFrames.emplace_back(Frame2D(glm::vec2(0.5f, 0.0f)));
//
//	LargeEnergyFlicker = Animation2D(LargeEnergyFlickerFrames, 0.25f, 0);
//	CurrentAni = LargeEnergyFlicker;
//
//	MeshTextures CoinTextures = {};
//	CoinTextures.DiffuseMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/LargeEnergy_diffuseOriginal.bmp";
//	CoinTextures.SpecularMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/LargeEnergy_Specular.bmp";
//	CoinTextures.NormalMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/LargeEnergy_normal.bmp";
//	CoinTextures.AlphaMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/LargeEnergy_Alpha.bmp";
//
//	ObjectFlagBits = ObjectFlags::Collectible;
//	RenderBitFlags = RenderBitFlag::RenderOnTexturePass | RenderBitFlag::RenderOnMainPass | RenderBitFlag::RenderMainPass;
//
//	SetUpSprite(renderer, textureManager, SpriteVertices, CoinTextures, StartPos);
//}
//
//LargeEnergy::~LargeEnergy()
//{
//}
//
//void LargeEnergy::Update(RendererManager& renderer, float dt, std::shared_ptr<Camera> camera, LightBufferObject light)
//{
//	CurrentAni.Update();
//	ObjectMesh->properites.UVOffset = CurrentAni.GetCurrentFrame().GetUVOffset();
//	ObjectMesh->properites.UVScale = CurrentAni.GetCurrentFrame().GetUVScale();
//
//	Sprite::Update(renderer, dt, camera, light);
//}
//
//void LargeEnergy::Collision(RendererManager& renderer, std::vector<std::shared_ptr<Object2D>>& ObjectList)
//{
//	for (auto obj : ObjectList)
//	{
//		if (dynamic_cast<MegaMan*>(obj.get()))
//		{
//			for (auto& collider : obj->ObjectColliderList)
//			{
//				if (collider->GetCollider().CollidesWith(ObjectColliderList[0]->GetCollider()))
//				{
//					DestoryObjectFlag = true;
//				}
//			}
//		}
//	}
//}
