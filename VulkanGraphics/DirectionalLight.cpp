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

	float near_plane = 0.1f;
	float far_plane = 100.0f;
	LightViewMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	LightProjectionMatrix = glm::lookAt(LightBuffer.UniformDataInfo.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update()
{
	Light::Update();

	float near_plane = 0.1f;
	float far_plane = 100.0f;
	LightViewMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	LightProjectionMatrix = glm::lookAt(LightBuffer.UniformDataInfo.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
}

void DirectionalLight::Destroy()
{
	Light::Destroy();
}
