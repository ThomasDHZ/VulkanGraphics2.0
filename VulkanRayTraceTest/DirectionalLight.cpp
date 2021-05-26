#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{

}

DirectionalLight::DirectionalLight(VulkanEngine& engine) : Light<DirectionalLightBuffer>(engine)
{
}

DirectionalLight::DirectionalLight(VulkanEngine& engine, CameraManager& cameraManager, DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(engine)
{
	auto ao = LightViewCamera(engine, glm::vec3(0.0f, 100.0f, 0.0f), LightBuffer.UniformDataInfo.direction);
	lightViewCamera = std::make_shared<LightViewCamera>(LightViewCamera(engine, glm::vec3(0.0f, 10.0f, 0.0f), LightBuffer.UniformDataInfo.direction));
	cameraManager.CameraList.emplace_back(lightViewCamera);

	LightBuffer.UniformDataInfo = light;
	Update(engine);
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update(VulkanEngine& engine)
{
	Light::Update(engine);
	lightViewCamera->Update(engine, glm::vec3(0.0f, 100.0f, 0.0f), LightBuffer.UniformDataInfo.direction);
}

void DirectionalLight::Destroy(VulkanEngine& engine)
{
	Light::Destroy(engine);
}
