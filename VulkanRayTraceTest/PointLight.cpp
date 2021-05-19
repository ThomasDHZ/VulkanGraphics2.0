#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(VulkanEngine& engine) : Light<PointLightBuffer>(engine)
{
}

PointLight::PointLight(VulkanEngine& engine, PointLightBuffer light) : Light<PointLightBuffer>(engine)
{
	LightBuffer.UniformDataInfo = light;
	Update(engine);
}

PointLight::~PointLight()
{
}
