#pragma once
#include "Model.h"

class ModelManager
{
private:
public:
	std::vector<std::shared_ptr<Model>> ModelList;
	void Update(VulkanEngine& engine, MaterialManager& materialManager);
};

