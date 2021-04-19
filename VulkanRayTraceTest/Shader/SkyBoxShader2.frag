  #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 10) uniform samplerCube CubeMap;

layout(location = 0) in vec3 TexCoords;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor2;
void main() 
{
    vec3 color = texture(CubeMap, TexCoords).rgb;
    FragColor = vec4(color, 1.0f);
    FragColor2 = vec4(color, 1.0f);
}