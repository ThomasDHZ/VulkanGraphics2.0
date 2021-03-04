#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Material {
    vec3 ambient;
    vec3 Diffuse;
    vec3 specular;    
    float shininess;
    float reflectivness;
}; 
struct DirectionalLightStruct {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLightStruct {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLightStruct {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

layout(binding = 1) uniform sampler2D DiffuseMap;
layout(binding = 2) uniform sampler2D SpecularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D AlphaMap;
layout(binding = 5) uniform sampler2D EmissionMap;
layout(binding = 6) uniform MeshProperties
{
    Material material;
   // MapBits mapBitsFlags;
   vec2 UVOffset;
        int UseDiffuseMapBit;
     int UseSpecularMapBit;
     int UseNormalMapBit;
     int UseDepthMapBit;
     int UseAlphaMapBit;
     int UseEmissionMapBit;
     int UseReflectionMapBit;
     int UseSkyBoxBit;
    float minLayers;
    float maxLayers;
    float heightScale;
    float timer;
    int ReflectSprite;
     vec2 UVScale;
    
} meshProperties;
layout(binding = 7) uniform Light
{
    DirectionalLightStruct dLight;
    PointLightStruct pLight;
    SpotLightStruct sLight;
    vec3 viewPos;
} light;
layout(binding = 8) uniform ExtendedMesh
{
    int empty;
} extended;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

void main() {
const float weights[] = float[](0.0024499299678342,
									0.0043538453346397,
									0.0073599963704157,
									0.0118349786570722,
									0.0181026699707781,
									0.0263392293891488,
									0.0364543006660986,
									0.0479932050577658,
									0.0601029809166942,
									0.0715974486241365,
									0.0811305381519717,
									0.0874493212267511,
									0.0896631113333857,
									0.0874493212267511,
									0.0811305381519717,
									0.0715974486241365,
									0.0601029809166942,
									0.0479932050577658,
									0.0364543006660986,
									0.0263392293891488,
									0.0181026699707781,
									0.0118349786570722,
									0.0073599963704157,
									0.0043538453346397,
									0.0024499299678342);

 vec2 tex_offset = 1.0 / textureSize(DiffuseMap, 0); // gets size of single texel
     vec3 result = texture(DiffuseMap, fragTexCoord).rgb * weights[0];

         for(int i = 1; i < weights.length(); ++i)
         {
            result += texture(DiffuseMap, fragTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(DiffuseMap, fragTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
         }

    outColor = vec4(result, 1.0f);
}