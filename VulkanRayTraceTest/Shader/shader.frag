#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform MeshInfo
{
	int MeshID;
	int MaterialID;
} Mesh;

layout(binding = 1) uniform sampler2D texSampler[];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {

   vec3 hdrColor = texture(texSampler[Mesh.MaterialID], fragTexCoord).rgb;
   vec3 result = vec3(1.0) - exp(-hdrColor * 1.0f);
   result = pow(result, vec3(1.0f / 2.2f));
   outColor = vec4(result, 1.0f);
}