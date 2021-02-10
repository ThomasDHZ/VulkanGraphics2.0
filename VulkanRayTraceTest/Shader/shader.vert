#version 450
#extension GL_ARB_separate_shader_objects : enable

struct DirectionalLight
{
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float constant;
    float linear;
    float quadratic;
};

layout(binding = 0) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	mat4 model;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	int vertexSize;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}

