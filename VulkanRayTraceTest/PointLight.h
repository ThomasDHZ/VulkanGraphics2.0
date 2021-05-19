#pragma once
#include "Light.h"
class PointLight : public Light<PointLightBuffer>
{
private:
public:
	PointLight();
	PointLight(VulkanEngine& engine);
	PointLight(VulkanEngine& engine, PointLightBuffer light);
	~PointLight();
};

