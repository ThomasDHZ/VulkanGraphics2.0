#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "Level2D.h"

class LavaTest : public Level2D
{
private:

public:
	LavaTest();
	LavaTest(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, glm::vec3 Position);
	~LavaTest();
};
