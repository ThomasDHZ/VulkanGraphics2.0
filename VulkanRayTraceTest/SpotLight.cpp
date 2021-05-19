#include "SpotLight.h"

SpotLight::SpotLight() : Light<SpotLightBuffer>()
{
}

SpotLight::SpotLight(VulkanEngine& engine) : Light<SpotLightBuffer>(engine)
{
}

SpotLight::~SpotLight()
{
}
