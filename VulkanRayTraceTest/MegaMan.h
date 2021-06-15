#pragma once
#include "Sprite.h"
#include "AssetManager.h"

class MegaMan : public Sprite
{
private:
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
		kShootClimbAnimation
	};

	MegaManAnimation CurrentAnimation;
public:

	static constexpr glm::vec2 SpriteSize = glm::vec2(1.0f);
	static constexpr glm::vec2 UVSize = glm::vec2(.05882f, 1.0f);

	MegaMan();
	MegaMan(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position);
	~MegaMan();

	void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer) override;
};

