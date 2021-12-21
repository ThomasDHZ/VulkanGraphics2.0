#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(PointLightBuffer light) : Light<PointLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<OrthographicLightViewCamera>(OrthographicLightViewCamera("PointShadowCamera", LightBuffer.UniformDataInfo.position, cameraDirection));
	CameraManagerPtr::GetCameraManagerPtr()->CameraList.emplace_back(lightViewCamera);
	auto a = CameraManagerPtr::GetCameraManagerPtr()->CameraList;
	Update();
}

PointLight::~PointLight()
{
}

void PointLight::Update()
{
	Light::Update();
	lightViewCamera->Update(LightBuffer.UniformDataInfo.position, cameraDirection);
}

void PointLight::Destroy()
{
	Light::Destroy();
}