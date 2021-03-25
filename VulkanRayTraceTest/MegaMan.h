#pragma once
#include "Sprite.h"
#include "AssetManager.h"

class MegaMan : public Sprite
{
private:
public:
	static constexpr glm::vec2 SpriteSize = glm::vec2(1.0f);
	static constexpr glm::vec2 UVSize = glm::vec2(.05f, 1.0f);

	MegaMan();
	MegaMan(VulkanEngine& engine, AssetManager& assetManager, glm::vec3 Position);
	~MegaMan();
};

