#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable


#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"


layout (location = 0) out vec2 UV;
void main() 
{
  UV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
  gl_Position = vec4(UV * 2.0f - 1.0f, 1.0f, 1.0f);
}
