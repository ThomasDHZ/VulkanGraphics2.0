#version 460
#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(binding = 10, set = 0) uniform samplerCube DiffuseMap;
void main()
{
    hitValue = texture(DiffuseMap, gl_WorldRayDirectionEXT).rgb;
}
