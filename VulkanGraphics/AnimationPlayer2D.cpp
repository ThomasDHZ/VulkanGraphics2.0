#include "AnimationPlayer2D.h"

AnimationPlayer2D::AnimationPlayer2D()
{
}

AnimationPlayer2D::AnimationPlayer2D(std::shared_ptr<Animation2D> animationList, glm::vec2 spriteUVSize, uint32_t spritesInSpriteSheet)
{
	AnimationIndex = 0;
	AnimationList = std::vector<std::shared_ptr<Animation2D>>{ animationList };
	SpriteUVSize = spriteUVSize;
	SpriesInSpriteSheet = spritesInSpriteSheet;
	CurrentAnimation = AnimationList[0];
	AnimationTimer = std::make_shared<Timer>(Timer());
}

AnimationPlayer2D::AnimationPlayer2D(std::vector<std::shared_ptr<Animation2D>> animationList, glm::vec2 spriteUVSize, uint32_t spritesInSpriteSheet, uint32_t DefaultAnimation)
{
	AnimationIndex = DefaultAnimation;
	AnimationList = animationList;
	SpriteUVSize = spriteUVSize;
	SpriesInSpriteSheet = spritesInSpriteSheet;
	CurrentAnimation = AnimationList[DefaultAnimation];
	AnimationTimer = std::make_shared<Timer>(Timer());
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

void AnimationPlayer2D::Update(bool FlipSpriteX)
{
	if (AnimationList.size() >= 1)
	{
		CurrentAnimation->Update(AnimationTimer, SpriteUVSize, SpriesInSpriteSheet, FlipSpriteX);
	}
}

bool AnimationPlayer2D::AnimationExistFlag()
{
	if (AnimationList.size() > 0)
	{
		return true;
	}
	return false;
}
