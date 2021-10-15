#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{

}

DirectionalLight::DirectionalLight(DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<OrthographicLightViewCamera>(OrthographicLightViewCamera(-LightBuffer.UniformDataInfo.direction));
	Update();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update()
{
	Light::Update();
	lightViewCamera->Update(-LightBuffer.UniformDataInfo.direction);
}

void DirectionalLight::Destroy()
{
	Light::Destroy();
}