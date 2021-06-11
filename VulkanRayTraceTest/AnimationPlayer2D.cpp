#include "AnimationPlayer2D.h"

AnimationPlayer2D::AnimationPlayer2D()
{
}

AnimationPlayer2D::AnimationPlayer2D(std::vector<std::shared_ptr<Animation2D>> animationList, uint32_t DefaultAnimation)
{
	AnimationList = animationList;
	CurrentAnimation = AnimationList[DefaultAnimation];
}

void AnimationPlayer2D::AddAnimation(std::shared_ptr<Animation2D> animation)
{
	AnimationList.emplace_back(animation);
}

void AnimationPlayer2D::AddAnimation(std::vector<std::shared_ptr<Animation2D>> animationList)
{
	for (auto& animation : animationList)
	{
		AnimationList.emplace_back(animation);
	}
}

void AnimationPlayer2D::SetAnimation(uint32_t AnimationIndex)
{
	CurrentAnimation = AnimationList[AnimationIndex];
}

void AnimationPlayer2D::Update(float timer)
{
	if (AnimationList.size() >= 1)
	{
		CurrentAnimation->Update(timer);
	}
}
