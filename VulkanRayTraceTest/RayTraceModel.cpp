#include "RayTraceModel.h"

RayTraceModel::RayTraceModel()
{
}

RayTraceModel::RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshContainer meshContainer)
{
	MeshList.emplace_back(RayTraceMesh(device, physicalDevice, meshContainer, glm::mat4(1.0f)));
}

RayTraceModel::~RayTraceModel()
{
}

void RayTraceModel::Update()
{
}
