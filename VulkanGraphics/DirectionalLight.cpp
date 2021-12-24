#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{

}

DirectionalLight::DirectionalLight(DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<OrthographicLightViewCamera>(OrthographicLightViewCamera("ShadowCamera", LightBuffer.UniformDataInfo.position, -LightBuffer.UniformDataInfo.direction));
	CameraManagerPtr::GetCameraManagerPtr()->CameraList.emplace_back(lightViewCamera);
	Update();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update()
{
	lightViewCamera->Update(-LightBuffer.UniformDataInfo.direction, LeftRight.x, LeftRight.y, TopBottom.x, TopBottom.y, NearFar.x, NearFar.y);
	glm::mat4 view = lightViewCamera->GetViewMatrix();
	glm::mat4 proj = lightViewCamera->GetProjectionMatrix();
	proj[1][1] *= -1;

	LightBuffer.UniformDataInfo.LightSpaceMatrix = proj * view;

	Light::Update();
}

void DirectionalLight::Destroy()
{
	Light::Destroy();
}
