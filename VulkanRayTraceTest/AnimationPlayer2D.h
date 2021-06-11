#pragma once
#include "Animation2D.h"

class AnimationPlayer2D
{
private:
	std::shared_ptr<Animation2D> CurrentAnimation;
	std::vector<std::shared_ptr<Animation2D>> AnimationList;
public:
	AnimationPlayer2D();
	AnimationPlayer2D(std::vector<std::shared_ptr<Animation2D>> AnimationList, uint32_t DefaultAnimation = 0);

	void AddAnimation(std::shared_ptr<Animation2D> animation);
	void AddAnimation(std::vector<std::shared_ptr<Animation2D>> AnimationList);
	void SetAnimation(uint32_t AnimationIndex);
	void Update(float timer);
	Frame2D GetCurrentFrame() { return CurrentAnimation->GetFrame(); };
};

