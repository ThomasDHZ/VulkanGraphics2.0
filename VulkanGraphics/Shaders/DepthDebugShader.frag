#version 460
layout(binding = 0) uniform sampler2D depthMap;
layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

float near_plane = 0.1f;
float far_plane = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main() 
{
   float depthValue = texture(depthMap, fragTexCoord).r;
    outColor = vec4(vec3(depthValue), 1.0);
}
