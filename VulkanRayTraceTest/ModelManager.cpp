#include "ModelManager.h"

void ModelManager::Update(VulkanEngine& engine)
{
	for (auto& model : ModelList)
	{
		model->Update(engine);
	}
}