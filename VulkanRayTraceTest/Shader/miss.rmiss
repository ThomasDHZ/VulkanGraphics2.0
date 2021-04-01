#version 460
#extension GL_EXT_ray_tracing : enable

layout(location = 1) rayPayloadInEXT vec3 hitValue;
layout(binding = 10, set = 0) uniform samplerCube CubeMap;
void main()
{
    hitValue = texture(CubeMap, gl_WorldRayDirectionEXT).rgb;
}
