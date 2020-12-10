#pragma once
#include "Sprite.h"
#include "SceneToTextureRendererPass.h"
class EnergyTank : public Sprite
{
private:

	static constexpr glm::vec2 SpriteSize = glm::vec2(0.5f, 0.5f);
	Animation2D EnergyTankFlicker;
public:
	EnergyTank();
	EnergyTank(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, glm::vec2 StartPos, VkDescriptorSetLayout& layout);
	~EnergyTank();

	void Update(VulkanEngine& engine, float dt, std::shared_ptr<PerspectiveCamera> camera, LightBufferObject light) override;
	void Collision(VulkanEngine& engine, std::vector<std::shared_ptr<Object2D>>& ObjectList) override;
};

