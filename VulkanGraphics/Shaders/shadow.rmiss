#version 460
#extension GL_EXT_ray_tracing : require
#extension SPV_KHR_ray_tracing : enable

layout(push_constant) uniform RayTraceConstants
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
    uint frame;
    int AntiAliasingCount;
    int MaxRefeflectCount;
    int  ApplyAntiAliasing;
} ConstData;

layout(location = 2) rayPayloadInEXT bool shadowed;

void main()
{
	shadowed = false;
}