  #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 10) uniform samplerCube CubeMap;

layout(location = 0) in vec3 WorldPos;

layout(location = 0) out vec4 FragColor;

void main() 
{
    vec3 worldPos = WorldPos;
    worldPos.x = -WorldPos.x;
    vec3 color = texture(CubeMap, worldPos).rgb;
    FragColor = vec4(color, 1.0f);
}