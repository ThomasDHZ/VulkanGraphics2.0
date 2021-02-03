#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(binding = 7, set = 0) uniform sampler3D DiffuseMap;

void main()
{
    hitValue = texture(DiffuseMap, gl_WorldRayDirectionEXT).rgb;
}
