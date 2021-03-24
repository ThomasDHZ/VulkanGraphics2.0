#include "ModelManager.h"

void ModelManager::Update(VulkanEngine& engine, MaterialManager& materialManager)
{
	for (auto& model : ModelList)
	{
		model->Update(engine, materialManager);
	}
}