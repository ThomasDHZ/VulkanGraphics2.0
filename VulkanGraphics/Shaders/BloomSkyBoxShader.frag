  #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 TexCoords;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

void main() 
{
    vec3 color = texture(skybox, TexCoords).rgb;
    FragColor = vec4(color, 1.0f);
     BloomColor = vec4(0.0f);
}