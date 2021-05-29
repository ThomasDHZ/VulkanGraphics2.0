  #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 10) uniform samplerCube CubeMap;

layout(location = 0) in vec3 TexCoords;

layout(location = 0) out vec4 GPosition;
layout(location = 1) out vec4 GSkyBox;
layout(location = 2) out vec4 GNormal;
layout(location = 3) out vec4 GTangent;
layout(location = 4) out vec4 GBiTangent;
layout(location = 5) out vec4 GBloom;
layout(location = 6) out vec4 NormalMap;
layout(location = 7) out vec4 SpecularMap;

void main() 
{
    vec3 color = texture(CubeMap, TexCoords).rgb;
    GPosition = vec4(color, 1.0f);
    GSkyBox = vec4(color, 1.0f);
    GNormal = vec4(color, 1.0f);
    GTangent = vec4(color, 1.0f);
    GBiTangent = vec4(color, 1.0f);
    GBloom = vec4(color, 1.0f);
    NormalMap = vec4(color, 1.0f);
    SpecularMap = vec4(color, 1.0f);
}