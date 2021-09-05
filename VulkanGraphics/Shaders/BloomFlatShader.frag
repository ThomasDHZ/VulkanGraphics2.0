#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform MeshColor
{
    vec3 color;
} meshColor;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

void main()
{           
    FragColor = vec4(meshColor.color, 0.25f);
    BloomColor = vec4(0.0f);
}