#pragma once
#include "Light.h"
class SpotLight : public Light<SpotLightBuffer>
{
private:
public:
	SpotLight();
	SpotLight(VulkanEngine& engine);
	SpotLight(VulkanEngine& engine, SpotLightBuffer light);
	~SpotLight();
};

