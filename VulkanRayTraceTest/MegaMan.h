#pragma once
#include "Sprite.h"
#include "AssetManager.h"

class MegaMan : public Sprite
{
private:
	enum MegaManStatus
	{
		kFaceRight = 1 << 0,
		kOnGround = 1 << 1,
		kMoving = 1 << 2,
		kShooting = 1 << 3,
		kClimping = 1 << 4,
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
		kLeftStandAnimation,
		kLeftStandToRunTransitionAnimation,
		kLeftRunAnimation,
		kLeftSlideAnimation,
		kLeftJumpAnimation,
		kLeftClimbAnimation,
		kLeftClimbUpAnimation,
		kLeftHurtAnimation,
		kLeftShootStandAnimation,
		kLeftShootRunAnimation,
		kLeftShootJumpAnimation,
		kLeftShootClimbAnimation
	};

	bool FlipSprite = false;
	bool MegaManStatus;
	MegaManAnimation CurrentAnimation;
public:

	static constexpr glm::vec2 SpriteSize = glm::vec2(1.0f);
	static constexpr glm::vec2 UVSize = glm::vec2(.05882f, 1.0f);

	MegaMan();
	MegaMan(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position);
	~MegaMan();

	void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer) override;
};

