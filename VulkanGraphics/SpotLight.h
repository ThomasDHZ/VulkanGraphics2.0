#pragma once
#include "Light.h"
class SpotLight : public Light<SpotLightBuffer>
{
private:
public:
	SpotLight();
	SpotLight(std::shared_ptr<VulkanEngine> engine);
	SpotLight(SpotLightBuffer light);
	~SpotLight();
};

