#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(std::shared_ptr<VulkanEngine> engine) : Light<PointLightBuffer>(engine)
{
}

PointLight::PointLight(std::shared_ptr<VulkanEngine> engine, PointLightBuffer light) : Light<PointLightBuffer>(engine)
{
	LightBuffer.UniformDataInfo = light;
	Update(engine);
}

PointLight::~PointLight()
{
}
