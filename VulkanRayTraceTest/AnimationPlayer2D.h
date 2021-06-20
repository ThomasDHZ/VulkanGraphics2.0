#pragma once
#include "Animation2D.h"

class AnimationPlayer2D
{
private:
	uint32_t AnimationIndex = 0;
	glm::ivec2 SingleSpriteSize = glm::ivec2(0);
	uint32_t SpriesInSpriteSheet = 0;
	std::shared_ptr<Animation2D> CurrentAnimation;
	std::vector<std::shared_ptr<Animation2D>> AnimationList;
public:
	AnimationPlayer2D();
	AnimationPlayer2D(std::vector<std::shared_ptr<Animation2D>> AnimationList, glm::ivec2 SingleSpriteSize, uint32_t SpriesInSpriteSheet, uint32_t DefaultAnimation = 0);

	void AddAnimation(std::shared_ptr<Animation2D> animation);
	void AddAnimation(std::vector<std::shared_ptr<Animation2D>> AnimationList);
	void SetAnimation(uint32_t AnimationIndex);
	void Update(std::shared_ptr<Timer> timer);
	std::shared_ptr<Animation2D> GetCurrentAnimation() { return CurrentAnimation; }
	uint32_t GetCurrentAnimationIndex() { return AnimationIndex; }
	uint32_t GetCurrentFrame() { return CurrentAnimation->GetCurrentFrame(); }
	Frame2D GetFrame() { return CurrentAnimation->GetFrame(); }
};

