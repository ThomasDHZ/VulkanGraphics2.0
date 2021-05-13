#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

layout(push_constant) uniform ConstBloomProperites
{
	uint BloomPass;
} BloomProperites;

layout(binding = 0) uniform sampler2D BloomTexture;

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

const float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
void main() 
{
    vec2 tex_offset = 1.0 / textureSize(BloomTexture, 0); // gets size of single texel
     vec3 result = texture(BloomTexture, TexCoords).rgb * weight[0];
     if(BloomProperites.BloomPass == 0)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(BloomTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(BloomTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(BloomTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
             result += texture(BloomTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
         }
     }
     outColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
}
