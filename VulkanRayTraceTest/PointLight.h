#pragma once
#include "Light.h"
class PointLight : public Light<PointLightBuffer>
{
private:
public:
	PointLight();
	PointLight(std::shared_ptr<VulkanEngine> engine);
	PointLight(std::shared_ptr<VulkanEngine> engine, PointLightBuffer light);
	~PointLight();
};

