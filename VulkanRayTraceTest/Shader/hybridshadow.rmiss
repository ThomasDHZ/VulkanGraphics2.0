#version 460
#extension GL_EXT_ray_tracing : require

layout(push_constant) uniform RayTraceCamera
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
    uint frame;
} Camera;
layout(location = 2) rayPayloadInEXT bool shadowed;

void main()
{
	shadowed = false;
}