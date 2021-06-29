#include "ModelManager.h"
#include "Level2D.h"

void ModelManager::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer, bool RayTraceFlag)
{
	for (auto& model : ModelList)
	{
		switch (model->ModelType)
		{
			case ModelTypeEnum::Model_Type_Normal: model->Update(engine, inputManager, materialManager, RayTraceFlag); break;
			case ModelTypeEnum::Model_Type_2D_Level: static_cast<Level2D*>(model.get())->Update(engine, inputManager, materialManager, timer); break;
			default: model->Update(engine, inputManager, materialManager, RayTraceFlag);
		};
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
