#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(VulkanEngine& engine) : Light<PointLightBuffer>(engine)
{
}

PointLight::~PointLight()
{
}
