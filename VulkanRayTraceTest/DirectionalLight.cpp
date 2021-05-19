#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{
}

DirectionalLight::DirectionalLight(VulkanEngine& engine) : Light<DirectionalLightBuffer>(engine)
{
}

DirectionalLight::DirectionalLight(VulkanEngine& engine, DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(engine)
{
	LightBuffer.UniformDataInfo = light;
	Update(engine);
}

DirectionalLight::~DirectionalLight()
{
}
