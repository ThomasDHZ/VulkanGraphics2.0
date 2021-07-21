#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform GUIProperties
{
	uint MaterialID;
	vec4 Color;
	vec2 Translation;
	vec2 Scale;
} ConstMesh;

layout(binding = 0) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 1) uniform sampler2D TextureMap[];

layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = texture(TextureMap[0], TexCoords);
}