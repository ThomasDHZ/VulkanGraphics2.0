#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 0) uniform sampler2D FrameBufferTexture;
layout(binding = 1) uniform sampler2D RTXShadowTexture;
layout(binding = 2) uniform sampler2D RTXReflectionTexture;
layout(binding = 3) uniform sampler2D RTXSSA0Texture;
layout(binding = 4) uniform sampler2D BloomTexture;
layout(binding = 5) uniform UniformBufferObject {
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

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

const float Gamma = 2.2f;
void main() 
{
    vec3 Color = texture(FrameBufferTexture, TexCoords).rgb;
    Color *= texture(RTXSSA0Texture, TexCoords).rgb;

//    vec3 lightDir = normalize(-scenedata.dlight.direction);
//    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
//            // specular
//            vec3 halfwayDir = normalize(lightDir + viewDir);  
//            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
//            vec3 specular = lights[i].Color * spec * Specular;

    if(texture(RTXShadowTexture, TexCoords).rgb == vec3(1.0f, 0.0f, 0.0f))
    {
        Color *= vec3(0.3f);
    }
    if(texture(RTXReflectionTexture, TexCoords).rgb != vec3(0.0f))
    {
        Color = texture(RTXReflectionTexture, TexCoords).rgb;
    }
    vec3 result = pow(Color, vec3(1.0 / Gamma));
    outColor = vec4(Color, 1.0f);
}
