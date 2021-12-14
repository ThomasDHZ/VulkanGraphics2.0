#version 460
#extension GL_EXT_ray_tracing : enable

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

struct RayHitInfo
{
	vec3 color;
	uint seed;
	vec3 normal;
    int reflectCount;
    bool hitGeo;
};

layout(location = 0) rayPayloadInEXT RayHitInfo rayPayload;
layout(binding = 11, set = 0) uniform samplerCube CubeMap[];
void main()
{
	rayPayload.color = texture(CubeMap[0], gl_WorldRayDirectionEXT).rgb;
	rayPayload.normal = vec3(0.0f);
}