#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;
layout (location = 5) in vec4 aColor;
layout (location = 6) in ivec4 BoneID;
layout (location = 7) in vec4 BoneWeights;

layout(location = 0) out vec2 fragTexCoord;

void main() 
{
	fragTexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(fragTexCoord * 2.0f - 1.0f, 0.0f, 1.0f);
}