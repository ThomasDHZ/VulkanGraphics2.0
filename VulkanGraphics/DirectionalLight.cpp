#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{

}

DirectionalLight::DirectionalLight(std::shared_ptr<VulkanEngine> engine) : Light<DirectionalLightBuffer>(engine)
{
}

DirectionalLight::DirectionalLight(DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	Update();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update()
{
	Light::Update();
}

void DirectionalLight::Destroy()
{
	Light::Destroy();
}
