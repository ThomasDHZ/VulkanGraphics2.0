#include "EnergyTank.h"
#include "MegaMan.h"
#include <algorithm>
#include <functional>

EnergyTank::EnergyTank() : Sprite()
{
}

EnergyTank::EnergyTank(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, glm::vec2 StartPos, VkDescriptorSetLayout& layout) : Sprite()
{
	const std::vector<Vertex> SpriteVertices =
	{
		{{0.0f, 0.0f, -0.1f},				 {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{SpriteSize.x, 0.0f, -0.1f},		 {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{SpriteSize.x, SpriteSize.y, -0.1f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{0.0f, SpriteSize.y, -0.1f},		 {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
	};

	std::vector<Frame2D> EnergyTankFlickerFrames;
	EnergyTankFlickerFrames.emplace_back(Frame2D(glm::vec2(0.0f, 0.0f)));
	EnergyTankFlickerFrames.emplace_back(Frame2D(glm::vec2(0.5f, 0.0f)));

	EnergyTankFlicker = Animation2D(EnergyTankFlickerFrames, 0.25f, 0);
	CurrentAni = EnergyTankFlicker;

	MeshTextures EnergyTank = {};
	EnergyTank.DiffuseMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/EnergyTank_diffuse.png";
	EnergyTank.SpecularMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/EnergyTank_Specular.png";
	EnergyTank.NormalMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/EnergyTank_Normal.png";
	EnergyTank.AlphaMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/EnergyTank_Alpha.png";
	EnergyTank.AlphaMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/EnergyTank_Bloom.png";

	ObjectFlagBits = ObjectFlags::Collectible;
	//RenderBitFlags = RenderBitFlag::RenderOnTexturePass | RenderBitFlag::RenderOnMainPass | RenderBitFlag::RenderMainPass;

	SetUpSprite(engine, textureManager, SpriteVertices, EnergyTank, StartPos, layout);
	//ObjectMesh->properites.ReflectSprite = true;
}

EnergyTank::~EnergyTank()
{
}

void EnergyTank::Update(VulkanEngine& engine, float dt, std::shared_ptr<Camera> camera, LightBufferObject light)
{
	/*CurrentAni.Update();
	ObjectMesh->properites.UVOffset = CurrentAni.GetCurrentFrame().GetUVOffset();
	ObjectMesh->properites.UVScale = CurrentAni.GetCurrentFrame().GetUVScale();*/

	Sprite::Update(engine, dt, camera, light);
}

void EnergyTank::Collision(VulkanEngine& engine, std::vector<std::shared_ptr<Object2D>>& ObjectList)
{
	for (auto obj : ObjectList)
	{
		if (dynamic_cast<MegaMan*>(obj.get()))
		{
			for (auto& collider : obj->ObjectColliderList)
			{
				if (collider->GetCollider().CollidesWith(ObjectColliderList[0]->GetCollider()))
				{
					DestoryObjectFlag = true;
				}
			}
		}
	}
}
