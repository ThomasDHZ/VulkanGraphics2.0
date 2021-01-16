#pragma once
#include "RayTraceMesh.h"

class RayTraceModel
{
private:
	glm::mat4 ModelTransformMatrix = glm::mat4(1.0f);

public:
	std::vector<RayTraceMesh> MeshList;
	
	glm::vec3 ModelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ModelRotate = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	RayTraceModel();
	RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshContainer meshContainer);
	~RayTraceModel();

	void Update();
};

