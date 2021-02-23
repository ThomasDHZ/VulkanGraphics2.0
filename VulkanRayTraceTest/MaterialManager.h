#pragma once
#include "VulkanEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Material
{
	alignas(16) glm::vec3 Ambient = glm::vec3(0.2f);
	alignas(16) glm::vec3 Diffuse = glm::vec3(0.6f);
	alignas(16) glm::vec3 Specular = glm::vec3(1.0f);
	alignas(4) float Shininess = 32;
	alignas(4) float Reflectivness = 0;

	alignas(4) uint32_t DiffuseMapID = 0;
	alignas(4) uint32_t SpecularMapID = 0;
	alignas(4) uint32_t NormalMapID = 0;
	alignas(4) uint32_t DepthMapID = 0;
	alignas(4) uint32_t AlphaMapID = 0;
	alignas(4) uint32_t EmissionMapID = 0;
	alignas(4) uint32_t ShadowMapID = 0;
};

class MaterialManager
{
private:
	std::vector<Material> MaterialList;

	uint32_t IsMateralLoaded(std::string name);
public:
};

