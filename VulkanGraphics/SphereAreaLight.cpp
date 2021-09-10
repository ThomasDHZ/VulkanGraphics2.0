#include "SphereAreaLight.h"

SphereAreaLight::SphereAreaLight() : Light<SphereAreaLightBuffer>()
{

}

SphereAreaLight::SphereAreaLight(std::shared_ptr<VulkanEngine> engine) : Light<SphereAreaLightBuffer>(engine)
{
}

SphereAreaLight::SphereAreaLight(SphereAreaLightBuffer light) : Light<SphereAreaLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	Update();
}

SphereAreaLight::~SphereAreaLight()
{
}

void SphereAreaLight::Update()
{
	Light::Update();
}

void SphereAreaLight::Destroy()
{
	Light::Destroy();
}
