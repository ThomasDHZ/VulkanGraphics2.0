#pragma once
#include "InputManager.h"
#include "Mesh.h"
#include "AnimationPlayer2D.h"

class Sprite : public Mesh
{
private:
protected:
	bool LastFlipSpriteX = false;
	bool FlipSpriteX = false;
	std::shared_ptr<Timer> timer2;
	AnimationPlayer2D AnimationPlayer;
	void AddAnimation(std::shared_ptr<Animation2D> animation);
	void AddAnimation(std::vector<std::shared_ptr<Animation2D>> AnimationList);
public:

	Sprite();
	Sprite(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material);
	~Sprite();

	void SetAnimation(uint32_t AnimationIndex);
	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer) override;
};

