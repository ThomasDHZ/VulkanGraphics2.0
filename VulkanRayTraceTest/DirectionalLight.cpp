#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{
}

DirectionalLight::DirectionalLight(VulkanEngine& engine) : Light<DirectionalLightBuffer>(engine)
{
}

DirectionalLight::~DirectionalLight()
{
}
