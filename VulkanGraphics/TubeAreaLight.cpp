#include "TubeAreaLight.h"

TubeAreaLight::TubeAreaLight() : Light<TubeAreaLightBuffer>()
{

}

TubeAreaLight::TubeAreaLight(std::shared_ptr<VulkanEngine> engine) : Light<TubeAreaLightBuffer>(engine)
{
}

TubeAreaLight::TubeAreaLight(TubeAreaLightBuffer light) : Light<TubeAreaLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	Update();
}

TubeAreaLight::~TubeAreaLight()
{
}

void TubeAreaLight::Update()
{
	Light::Update();
}

void TubeAreaLight::Destroy()
{
	Light::Destroy();
}
