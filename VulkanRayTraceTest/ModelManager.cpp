#include "ModelManager.h"

void ModelManager::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer)
{
	for (auto& model : ModelList)
	{
		model->Update(engine, inputManager, materialManager, timer);
	}
}

std::shared_ptr<Model> ModelManager::GetModelIndex(uint32_t ModelID)
{
	std::shared_ptr<Model> modelptr;
	for (auto& model : ModelList)
	{
		if (ModelID == model->ModelID)
		{
			modelptr = model;
		}
	}

	return modelptr;
}
