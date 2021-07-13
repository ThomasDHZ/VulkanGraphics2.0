#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoords;

layout(push_constant) uniform TextConsts
{
    mat4 proj;
} text;

layout(location = 0) out vec2 fragTexCoord;
void main() 
{
    gl_Position = text.proj * vec4(Position.xy, 0.0, 1.0);
    fragTexCoord = TexCoords;
}