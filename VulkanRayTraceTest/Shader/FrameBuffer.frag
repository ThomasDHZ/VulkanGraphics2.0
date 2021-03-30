#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 2) uniform UniformBufferObject {
	DirectionalLight dlight;
	PointLight plight[5];
	SpotLight sLight;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
	float timer;
    int Shadowed;
    int temp;
} scenedata;

layout(binding = 8) uniform sampler2D TextureMap[];

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;


void main() 
{
    outColor = vec4(texture(TextureMap[scenedata.temp], TexCoords));
}
