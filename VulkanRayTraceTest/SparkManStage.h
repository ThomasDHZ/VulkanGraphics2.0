#pragma once
#include "AssetManager.h"
#include "Level2D.h"

class SparkManStage : public Level2D
{
private:

public:
	SparkManStage();
	SparkManStage(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position);
	~SparkManStage();
};

