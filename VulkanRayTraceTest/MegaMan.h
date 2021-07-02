#pragma once
#include "Sprite.h"
#include "AssetManager.h"

class MegaMan : public Sprite
{
private:
	enum MegaManStatus
	{
		kFlipX = 1 << 0,
		kOnGround = 1 << 1,
		kMoving = 1 << 2,
		kShooting = 1 << 3,
		kClimbing = 1 << 4,
		kSliding = 1 << 5,
		kHurt = 1 << 6
	};
	enum MegaManAnimation
	{
		kStandAnimation,
		kStandToRunTransitionAnimation,
		kRunAnimation,
		kSlideAnimation,
		kJumpAnimation,
		kClimbAnimation,
		kClimbUpAnimation,
		kHurtAnimation,
		kShootStandAnimation,
		kShootRunAnimation,
		kShootJumpAnimation,
		kShootClimbAnimation,
	};
	float Gravity = 0.098f;
	int megaManStatus = 0;
	MegaManAnimation CurrentAnimation;
public:
	static constexpr uint32_t SpritesInSpriteSheet = 17; //In Pixels;
	static constexpr glm::vec2 SpriteSize = glm::vec2(1.0f);
	static constexpr glm::vec2 SpriteUVSize = glm::vec2(.05882f, 1.0f);

	MegaMan();
	MegaMan(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position);
	~MegaMan();

	void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer, std::vector<std::shared_ptr<LevelTile>> LevelTileLayout, std::vector<std::shared_ptr<Mesh>> MeshList);
};

