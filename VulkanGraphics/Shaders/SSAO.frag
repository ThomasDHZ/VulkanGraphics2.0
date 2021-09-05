#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D gPositionTexture;
layout(binding = 1) uniform sampler2D gNormalTexture;
layout(binding = 2) uniform sampler2D NoiseTexture;
layout(binding = 3) uniform SSAOConfig
{
    mat4 Projection;
    vec3 Samples[64];
    int KernelSize;
    float Radius;
    float Bias;
} config;

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

void main() 
{
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPositionTexture, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormalTexture, TexCoords).rgb);
    vec3 randomVec = normalize(texture(NoiseTexture, TexCoords * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < config.KernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * config.Samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * config.Radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = config.Projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPositionTexture, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, config.Radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + config.Bias ? 1.0 : 0.0) * rangeCheck;           
    }

    occlusion = 1.0 - (occlusion / config.KernelSize);
    outColor = vec4(texture(gNormalTexture, TexCoords).rgb, 1.0f);
}