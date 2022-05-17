#include "SpotLight.h"

SpotLight::SpotLight() : Light<SpotLightBuffer>()
{
}

SpotLight::SpotLight(std::shared_ptr<VulkanEngine> engine) : Light<SpotLightBuffer>(engine)
{
}

SpotLight::SpotLight(SpotLightBuffer light) : Light<SpotLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<PerspectiveCamera>(PerspectiveCamera("SpotLightCamera", LightBuffer.UniformDataInfo.position, -LightBuffer.UniformDataInfo.direction));
	CameraManagerPtr::GetCameraManagerPtr()->CameraList.emplace_back(lightViewCamera);
	Update();
}

SpotLight::~SpotLight()
{
}

void SpotLight::Update()
{
	lightViewCamera->Update(EnginePtr::GetEnginePtr());

	glm::mat4 view = lightViewCamera->GetViewMatrix();
	glm::mat4 proj = lightViewCamera->GetProjectionMatrix();
	proj[1][1] *= -1;

	LightBuffer.UniformDataInfo.LightSpaceMatrix = proj * view;

	Light::Update();
}

void SpotLight::Destroy()
{
	Light::Destroy();
}