#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout(location = 0) out vec2 TexCoords;

void main() 
{
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
    TexCoords = aTexCoords;
}
