#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{

}

DirectionalLight::DirectionalLight(std::shared_ptr<VulkanEngine> engine) : Light<DirectionalLightBuffer>(engine)
{
}

DirectionalLight::DirectionalLight(DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(EnginePtr::GetEnginePtr())
{
	lightViewCamera = std::make_shared<LightViewCamera>(LightViewCamera(EnginePtr::GetEnginePtr(), glm::vec3(0.0f, 10.0f, 0.0f), LightBuffer.UniformDataInfo.direction));
	CameraManagerPtr::GetCameraManagerPtr()->CameraList.emplace_back(lightViewCamera);

	LightBuffer.UniformDataInfo = light;
	Update();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update()
{
	Light::Update();
	lightViewCamera->Update(EnginePtr::GetEnginePtr(), glm::vec3(0.0f, 100.0f, 0.0f), LightBuffer.UniformDataInfo.direction);
}

void DirectionalLight::Destroy()
{
	Light::Destroy();
}
