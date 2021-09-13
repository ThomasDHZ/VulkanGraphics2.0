#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "material.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform UniformBufferObject {
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    uint SphereAreaLightCount;
    uint TubeAreaLightCount;
    uint RectangleAreaLightCount;
	float timer;
    int Shadowed;
    int temp;
} scenedata;

layout(binding = 1) buffer MeshProperties 
{
	mat4 ModelTransform;
	vec2 UVOffset;
    vec2 UVScale;
    vec2 UVFlip;
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];

layout(binding = 2) buffer DirectionalLight
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float Luminosity;
} DLight[];

layout(binding = 3) buffer PointLight
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float Luminosity;
} PLight[];

layout(binding = 4) buffer SpotLight
{ 
   vec3 position;
   vec3 direction;
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float cutOff;
   float outerCutOff;
   float constant;
   float linear;
   float quadratic;
   float Luminosity;
} SLight[];

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer Material { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap;

layout(binding = 10) buffer SphereAreaLightBuffer {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float SphereRadius;
	float Luminosity;
} SphereLight[];

layout(binding = 11) buffer TubeAreaLightBuffer {
	vec3 StartPos;
	vec3 EndPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    float TubeRadius;
	float Luminosity;
} TubeLight[];

layout(binding = 12) buffer RectangleAreaLightBuffer
{
	vec3 VertexPos1;
	vec3 VertexPos2;
	vec3 VertexPos3;
	vec3 VertexPos4;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float Luminosity;
} RectangleLight[];

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

MaterialInfo material;
mat3 TBN;

vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, vec2 uv, int index);
vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, vec2 uv, int index);
vec3 CalcSphereAreaLight(vec3 FragPos, vec3 normal, vec2 uv, int index);
vec3 CalcTubeAreaLight(vec3 normal, vec2 uv, int index);

void main() 
{
   material = MaterialList[meshProperties[Mesh.MeshIndex].MaterialIndex].material;
   vec2 texCoords = TexCoords + meshProperties[Mesh.MeshIndex].UVOffset;
   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
   {
	 discard;
   }
   texCoords *= meshProperties[Mesh.MeshIndex].UVScale;
   if(meshProperties[Mesh.MeshIndex].UVFlip.y == 1.0f)
   {
        texCoords.y = 1.0f - texCoords.y;
   }
   if(meshProperties[Mesh.MeshIndex].UVFlip.x == 1.0f)
   {
        texCoords.x = 1.0f - texCoords.x;
   }
   if(meshProperties[Mesh.MeshIndex].UVFlip.y == 1.0f)
   {
        texCoords.y = 1.0f - texCoords.y;
   }
   
   vec3 result = vec3(0.0f);
   vec3 normal = Normal;
   vec3 ViewPos  = Mesh.CameraPos;
   vec3 FragPos2  = FragPos;
   
   for(int x = 0; x < scenedata.DirectionalLightCount; x++)
   {
      //  result += CalcNormalDirLight(FragPos2, normal, texCoords, x);
   }
   for(int x = 0; x < scenedata.PointLightCount; x++)
   {
       // result += CalcNormalPointLight(FragPos2, normal, texCoords, x);   
   }
//      for(int x = 0; x < scenedata.PointLightCount; x++)
//   {
//        result += CalcNormalPointLight(FragPos2, normal, texCoords, x);   
//   }
   for(int x = 0; x < scenedata.SphereAreaLightCount; x++)
   {
        result += CalcSphereAreaLight(FragPos2, normal, texCoords, x);
   }
   for(int x = 0; x < scenedata.TubeAreaLightCount; x++)
   {
       // result += CalcTubeAreaLight(normal, texCoords, x);   
   }
      for(int x = 0; x < scenedata.RectangleAreaLightCount; x++)
   {
      //  result += CalcNormalPointLight(FragPos2, normal, texCoords, x);   
   }

    vec3 I = normalize(FragPos2 - ViewPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 Reflection = texture(CubeMap, R).rgb;
    vec3 finalMix = mix(result, Reflection, material.Reflectivness);

    outColor = vec4(result, 1.0f);
    outBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if(material.DiffuseMapID != 0)
    {
        if(texture(TextureMap[material.DiffuseMapID], texCoords).r >= 0.8f)
        {
          outBloom = vec4(texture(TextureMap[material.DiffuseMapID], texCoords).rgb, 1.0f);
        }
    }
    else
    {
        if(material.Diffuse.r >= 0.8f)
        {
           outBloom = vec4(material.Diffuse, material.Alpha);
        }
    }
}

vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = DLight[index].direction;
    vec3 ViewPos = Mesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[index].direction;
        ViewPos = TBN * Mesh.CameraPos;
        FragPos2 = TBN * FragPos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    const vec3 lightDir = normalize(-LightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + ViewDir);
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = DLight[index].ambient * material.Diffuse.rgb;
    vec3 diffuse = DLight[index].diffuse * diff * material.Diffuse.rgb;
    vec3 specular = DLight[index].specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = DLight[index].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = DLight[index].diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = DLight[index].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float LightDistance = length(LightPos - FragPos2);
    float LightIntensity = DLight[index].Luminosity / (LightDistance * LightDistance);

    return (ambient + diffuse + specular) * LightIntensity;
}

vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = PLight[index].position;
    vec3 ViewPos = Mesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * PLight[index].position;
        ViewPos = TBN * Mesh.CameraPos;
        FragPos2 = TBN * FragPos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    vec3 lightDir = normalize(LightPos - FragPos2);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = PLight[index].ambient * material.Diffuse.rgb;
    vec3 diffuse = PLight[index].diffuse * diff * material.Diffuse.rgb;
    vec3 specular = PLight[index].specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = PLight[index].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = PLight[index].diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = PLight[index].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float LightDistance = length(LightPos - FragPos2);
    float attenuation = 1.0 / (PLight[index].constant + PLight[index].linear * LightDistance + PLight[index].quadratic * (LightDistance * LightDistance));
    float LightIntensity = PLight[index].Luminosity / (LightDistance * LightDistance);

    return (ambient + diffuse + specular) * attenuation * LightIntensity;
}

vec3 CalcSphereAreaLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = SphereLight[index].position;
    vec3 ViewPos = Mesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * SphereLight[index].position;
        ViewPos = TBN * Mesh.CameraPos;
        FragPos2 = TBN * FragPos;
    }

    vec3 R = reflect(-ViewPos, normal);

    vec3 L = normalize(ViewPos - FragPos2);
    vec3 CenterToRay = (dot(L,R)*R)-L;
    vec3 ClosestPoint = L + CenterToRay * clamp(SphereLight[index].SphereRadius / length(CenterToRay), 0.0f, 1.0f);
    L = ClosestPoint;
    float DistLight = length(ClosestPoint);

    vec3 lightDir = normalize(LightPos - FragPos2);
    vec3 H = normalize(lightDir + L);
    float spec = pow(max(dot(normal, H), 0.0), material.Shininess);

    vec3 ambient = SphereLight[index].ambient * material.Diffuse.rgb;
    vec3 diffuse = SphereLight[index].diffuse * DistLight * material.Diffuse.rgb;
    vec3 specular = SphereLight[index].specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = SphereLight[index].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = SphereLight[index].diffuse * DistLight * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = SphereLight[index].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float LightDistance = length(LightPos - FragPos2);
    float falloff = pow(clamp(1.0 - pow(LightDistance/(SphereLight[index].SphereRadius), 4), 0.0, 1.0f), 2) / ((LightDistance * LightDistance) + 1.0);	
    float LightIntensity = SphereLight[index].Luminosity / (LightDistance * LightDistance);

    return (ambient + diffuse + specular) * falloff * LightIntensity;
}

//vec3 CalcTubeAreaLight(vec3 normal, vec2 uv, int index)
//{
//    vec3 StartPos = TubeLight[index].StartPos;
//    vec3 EndPos = TubeLight[index].EndPos;
//    vec3 ViewPos = Mesh.CameraPos;
//    vec3 FragPos2 = FragPos;
//    if (material.NormalMapID != 0)
//    {
//        ViewPos = TBN * Mesh.CameraPos;
//        FragPos2 = TBN * FragPos;
//        StartPos = TBN * StartPos;
//        EndPos = TBN * EndPos;
//    }
//    vec3 R = reflect(-ViewPos, normal);
//    
//    vec3 LightStartPos = StartPos - ViewPos;
//    vec3 LightEndPos = EndPos - ViewPos;
//
//    float StartPosDistance = length(LightStartPos);
//    float EndPosDistance = length(LightEndPos);
//
//    float StartPosNOL = dot(LightStartPos, normal)/(2.0f*StartPosDistance);
//    float EndPosNOL = dot(LightEndPos, normal)/(2.0f*EndPosDistance);
//    float NoL = ( 2.0 * clamp( StartPosNOL + EndPosNOL, 0.0f, 1.0f)) / ( StartPosNOL * EndPosNOL + dot( StartPosDistance, EndPosDistance ) + 2.0 );
//    vec3 LightDistance = LightEndPos - LightStartPos;
//    
//    float RoLd = dot(R, LightDistance);
//    float DistLD = length(LightDistance);
//    float t = ( dot(R, LightStartPos ) * RoLd - dot( LightStartPos, LightDistance) ) / ( DistLD * DistLD - RoLd * RoLd );
//
//    vec3 ClosestPoint = LightStartPos + LightDistance * clamp(t, 0.0f, 1.0f);
//    vec3 CenterToRay = dot(ClosestPoint, R) * R - ClosestPoint;
//    CenterToRay = CenterToRay * clamp(CenterToRay / length(CenterToRay), 0.0f, 1.0f);
//    
//    vec3 L = normalize(ClosestPoint);
//    float DistLight = length(ClosestPoint);
//
//    vec3 lightDir = normalize(L - FragPos2);
//    vec3 H = normalize(lightDir + L);
//    float spec = pow(max(dot(normal, H), 0.0), material.Shininess);
//
//    vec3 ambient = SphereLight[index].ambient * material.Diffuse.rgb;
//    vec3 diffuse = SphereLight[index].diffuse * DistLight * material.Diffuse.rgb;
//    vec3 specular = SphereLight[index].specular * spec * material.Specular;
//    if (material.DiffuseMapID != 0)
//    {
//        ambient = SphereLight[index].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
//        diffuse = SphereLight[index].diffuse * DistLight * vec3(texture(TextureMap[material.DiffuseMapID], uv));
//    }
//    if (material.SpecularMapID != 0)
//    {
//        specular = SphereLight[index].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
//    }
//
//    float falloff = pow(clamp(1.0 - pow(DistLD/(TubeLight[index].TubeRadius), 4), 0.0, 1.0f), 2) / ((DistLD * DistLD) + 1.0);	
//    float LightIntensity = TubeLight[index].Luminosity / (DistLD * DistLD);
////
//    return (ambient + diffuse + specular) * falloff * LightIntensity;
//}