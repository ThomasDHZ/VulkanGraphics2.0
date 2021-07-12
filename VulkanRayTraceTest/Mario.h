#pragma once
#include "Sprite.h"
#include "AssetManager.h"

class Mario : public Sprite
{
private:
public:
	static constexpr glm::ivec2 SingleSpriteSize = glm::ivec2(32); //In Pixels;
	static constexpr uint32_t SpritesInSpriteSheet = 17; //In Pixels;
	static constexpr glm::vec2 SpriteSize = glm::vec2(1.0f);
	static constexpr glm::vec2 UVSize = glm::vec2(.05f, 1.0f);

	Mario();
	Mario(VulkanEngine& engine, AssetManager& assetManager, glm::vec3 Position);
	~Mario();

	void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager) override;
};

