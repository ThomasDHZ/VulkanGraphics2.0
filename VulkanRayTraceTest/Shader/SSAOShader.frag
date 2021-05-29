#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(push_constant) uniform SSAOProperties
{
    mat4 projection;
    int kernelSize;
    float radius;
    float bias;
    float TextureWidth;
    float TextureHeight;
} properties;

layout(binding = 0) uniform sampler2D GPositionTexture;
layout(binding = 1) uniform sampler2D GNormalTexture;
layout(binding = 2) uniform sampler2D NormalMapTexture;
layout(binding = 3) uniform sampler2D NoiseTexture;
layout(binding = 4) buffer SSAOSample { vec3 SSAOSample; } SSAOBuffer[];

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

void main() 
{
    vec2 noiseScale = vec2(properties.TextureWidth/4.0, properties.TextureHeight/4.0); 
// get input for SSAO algorithm
    vec3 fragPos = texture(GPositionTexture, TexCoords).xyz;
    vec3 normal = normalize(texture(GNormalTexture, TexCoords).rgb);
    if(texture(NormalMapTexture, TexCoords).a == 1.0f)
    {
//        if(material.DepthMapID != 0)
//        {
//            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
//            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//            {
//              discard;
//            }
//        }
        normal = texture(NormalMapTexture, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
     }

    vec3 randomVec = normalize(texture(NoiseTexture, TexCoords * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < properties.kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * SSAOBuffer[i].SSAOSample; // from tangent to view-space
        samplePos = fragPos + samplePos * properties.radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = properties.projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(GPositionTexture, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, properties.radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + properties.bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / properties.kernelSize);
    
    outColor = vec4(vec3(occlusion), 1.0f);
}