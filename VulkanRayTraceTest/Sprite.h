#pragma once
#include "InputManager.h"
#include "Mesh.h"
#include "AnimationPlayer2D.h"
#include "TileCollider.h"
#include "Tile.h"
#include "LevelTile.h"

class Sprite : public Mesh
{
private:
protected:
	bool LastFlipSpriteX = false;
	bool FlipSpriteX = false;
	AnimationPlayer2D AnimationPlayer;

	void AddAnimation(std::shared_ptr<Animation2D> animation);
	void AddAnimation(std::vector<std::shared_ptr<Animation2D>> AnimationList);
public:
	glm::vec2 Velocity = glm::vec2(0.0f);
	glm::vec2 SpriteSize = glm::vec2(0.0f);

	Sprite();
	Sprite(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material);
	~Sprite();

	TileCollider tileCollider;
	void SetAnimation(uint32_t AnimationIndex);
	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, std::vector<std::shared_ptr<LevelTile>> LevelTileLayout, std::vector<std::shared_ptr<Mesh>> MeshList);
};

