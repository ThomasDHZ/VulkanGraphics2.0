#include "AnimationPlayer2D.h"

AnimationPlayer2D::AnimationPlayer2D()
{
}

AnimationPlayer2D::AnimationPlayer2D(std::vector<std::shared_ptr<Animation2D>> animationList, glm::vec2 spriteUVSize, uint32_t spritesInSpriteSheet, uint32_t DefaultAnimation)
{
	AnimationIndex = DefaultAnimation;
	AnimationList = animationList;
	SpriteUVSize = spriteUVSize;
	SpriesInSpriteSheet = spritesInSpriteSheet;
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

void AnimationPlayer2D::SetAnimation(uint32_t animationIndex)
{
	AnimationIndex = animationIndex;
	CurrentAnimation = AnimationList[animationIndex];
}

void AnimationPlayer2D::Update(std::shared_ptr<Timer> timer, bool FlipSpriteX)
{
	if (AnimationList.size() >= 1)
	{
		CurrentAnimation->Update(timer, SpriteUVSize, SpriesInSpriteSheet, FlipSpriteX);
	}
}
