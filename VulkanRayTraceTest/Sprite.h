#pragma once
#include "Mesh.h"
#include "AnimationPlayer2D.h"

class Sprite : public Mesh
{
private:
	AnimationPlayer2D AnimationPlayer;
protected:
	void AddAnimation(std::shared_ptr<Animation2D> animation);
	void AddAnimation(std::vector<std::shared_ptr<Animation2D>> AnimationList);
public:

	Sprite();
	Sprite(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material);
	~Sprite();

	virtual void Update(VulkanEngine& engine, MaterialManager& materialManager, float timer) override;
};

