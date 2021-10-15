#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(std::shared_ptr<VulkanEngine> engine) : Light<PointLightBuffer>(engine)
{
}

PointLight::PointLight(PointLightBuffer light) : Light<PointLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	Update();
}

PointLight::~PointLight()
{
}
