#include "RectangleAreaLight.h"

RectangleAreaLight::RectangleAreaLight() : Light<RectangleAreaLightBuffer>()
{

}

RectangleAreaLight::RectangleAreaLight(std::shared_ptr<VulkanEngine> engine) : Light<RectangleAreaLightBuffer>(engine)
{
}

RectangleAreaLight::RectangleAreaLight(RectangleAreaLightBuffer light) : Light<RectangleAreaLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	Update();
}

RectangleAreaLight::~RectangleAreaLight()
{
}

void RectangleAreaLight::Update()
{
	Light::Update();
}

void RectangleAreaLight::Destroy()
{
	Light::Destroy();
}
