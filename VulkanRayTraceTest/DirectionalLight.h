#pragma once
#include "Light.h"

class DirectionalLight : public Light<DirectionalLightBuffer>
{
private:
public:
	DirectionalLight();
	DirectionalLight(VulkanEngine& engine);
	DirectionalLight(VulkanEngine& engine, DirectionalLightBuffer light);
	~DirectionalLight();
};

