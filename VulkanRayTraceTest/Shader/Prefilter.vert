  #version 450
#extension GL_ARB_separate_shader_objects : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform ConstSkyBoxView
{
	mat4 view;
	mat4 proj;
	float MipLevel;
	uint SampleCount;
	float FaceSize;
} skybox;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;
layout (location = 5) in vec4 aColor;
layout (location = 6) in ivec4 BoneID;
layout (location = 7) in vec4 BoneWeights;

layout(location = 0) out vec3 TexCoords;

void main() {
	TexCoords = aPos;
	vec4 pos = skybox.proj * skybox.view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
