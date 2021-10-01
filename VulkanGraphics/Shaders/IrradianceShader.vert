  #version 450
#extension GL_ARB_separate_shader_objects : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform CubeMapView
{
    mat4 proj;
    mat4 view;
} view;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

layout(location = 0) out vec3 WorldPos;

void main() {
	WorldPos = aPos;
	vec4 pos = view.proj * view.view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
