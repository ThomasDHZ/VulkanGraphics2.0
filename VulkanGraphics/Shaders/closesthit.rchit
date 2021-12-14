#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "SceneProperties.glsl"
#include "MeshProperties.glsl"
#include "Vertex.glsl"
#include "Lighting.glsl"

layout(push_constant) uniform RayTraceConstants
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
    uint frame;
    int AntiAliasingCount;
    int MaxRefeflectCount;
    int  ApplyAntiAliasing;
} ConstMesh;

struct RayPayload {
	vec3 color;
	float seed;
	vec3 normal;
    int reflectCount;
};

layout(location = 0) rayPayloadInEXT RayPayload rayHitInfo;
layout(location = 1) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 3) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];
layout(binding = 4) buffer DirectionalLightBuffer{ DirectionalLight directionalLight; } DLight[];
layout(binding = 5) buffer PointLightBuffer { PointLight pointLight; } PLight[];
layout(binding = 6) buffer SpotLightBuffer { SpotLight spotLight; } SLight[];
layout(binding = 7, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 8) buffer Indices { uint i[]; } indices[];
layout(binding = 9) uniform sampler2D TextureMap[];
layout(binding = 10) uniform sampler3D Texture3DMap[];
layout(binding = 11) uniform samplerCube CubeMap[];

Vertex vertex;
MaterialInfo material;
mat3 TBN;

#include "RTVertexBuilder.glsl"
#include "MaterialBuilder.glsl"

vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, vec2 uv, int index);
vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, vec2 uv, int index);
vec3 CalcNormalSpotLight(vec3 FragPos, vec3 normal, vec2 uv, int index);
vec2 ParallaxMapping( MaterialInfo material, vec2 texCoords, vec3 viewDir);
vec3 RTXShadow(vec3 LightResult, vec3 LightSpecular, vec3 LightDirection, float LightDistance);

void main()
{
   vertex = BuildVertexInfo();
   material = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.material;
   const vec3 T = normalize(mat3(meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.ModelTransform * meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.MeshTransform) * vec3(vertex.tangent));
   const vec3 B = normalize(mat3(meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.ModelTransform * meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.MeshTransform) * vec3(vertex.BiTangant));
   const vec3 N = normalize(mat3(meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.ModelTransform * meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.MeshTransform) * vertex.normal);
   TBN = transpose(mat3(T, B, N));

    vec3 result = vec3(0.0f);
    vec3 baseColor = vec3(0.0f);
    vec3 normal = vertex.normal;
    vec3 ViewPos  = ConstMesh.CameraPos;
    vec3 FragPos  = vertex.pos;
    const vec3 viewDir = normalize(ViewPos - FragPos);

    if(material.NormalMapID != 0)
    {
        ViewPos  = TBN * ConstMesh.CameraPos;
        FragPos  = TBN * FragPos;
    }

    if(material.NormalMapID != 0)
    {
        normal = texture(TextureMap[material.NormalMapID], vertex.uv).rgb;
        normal = normalize(normal * 2.0 - 1.0);
     }

    for(int x = 0; x < sceneBuffer.sceneData.DirectionalLightCount; x++)
     {
        baseColor += CalcNormalDirLight(FragPos, normal, vertex.uv, x);
     }
//     for(int x = 0; x < sceneBuffer.sceneData.PointLightCount; x++)
//     {
//        baseColor += CalcNormalPointLight(FragPos, normal, vertex.uv, x);   
//     }
//       for(int x = 0; x < sceneBuffer.sceneData.SpotLightCount; x++)
//       {
//            baseColor += CalcNormalSpotLight(FragPos, normal, vertex.uv, x);   
//       }


    if(material.Reflectivness > 0.0f &&
       rayHitInfo.reflectCount != ConstMesh.MaxRefeflectCount)
    {
        vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
        vec3 origin   = hitPos.xyz + normal * 0.001f;
        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, normal);

        rayHitInfo.reflectCount++;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
		result = mix(baseColor, rayHitInfo.color, material.Reflectivness); 
    }
    else
	{
        result = baseColor;
        rayHitInfo.reflectCount = ConstMesh.MaxRefeflectCount + 1;
	}

    rayHitInfo.color = result;
	//rayHitInfo.distance = gl_RayTmaxNV;
	rayHitInfo.normal = vertex.normal;
}

vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = DLight[index].directionalLight.direction;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = vertex.pos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[index].directionalLight.direction;
        ViewPos = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * vertex.pos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    const vec3 lightDir = normalize(-LightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + ViewDir);
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = DLight[index].directionalLight.ambient * material.Diffuse.rgb;
    vec3 diffuse = DLight[index].directionalLight.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = DLight[index].directionalLight.specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = DLight[index].directionalLight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
        diffuse = DLight[index].directionalLight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = DLight[index].directionalLight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], vertex.uv));
    }

    float LightDistance = length(LightPos - FragPos2);
  //  float LightIntensity = DLight[index].Luminosity / (LightDistance * LightDistance);

    vec3 result = (ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, 10000.0f);
    return result;
}

vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = PLight[index].pointLight.position;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = vertex.pos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * PLight[index].pointLight.position;
        ViewPos = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * vertex.pos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    vec3 lightDir = normalize(LightPos - FragPos2);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = PLight[index].pointLight.ambient * material.Diffuse.rgb;
    vec3 diffuse = PLight[index].pointLight.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = PLight[index].pointLight.specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = PLight[index].pointLight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
        diffuse = PLight[index].pointLight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = PLight[index].pointLight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], vertex.uv));
    }

    float LightDistance = length(LightPos - FragPos2);
    float attenuation = 1.0 / (1.0f + PLight[index].pointLight.linear * LightDistance + PLight[index].pointLight.quadratic * (LightDistance * LightDistance));

    vec3 result = (ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, 10000.0f);
    result *= attenuation;
    return result;
}

vec3 CalcNormalSpotLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = SLight[index].spotLight.position;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = vertex.pos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * SLight[index].spotLight.position;
        ViewPos = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * vertex.pos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    vec3 lightDir = normalize(LightPos - FragPos2);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = SLight[index].spotLight.ambient * material.Diffuse.rgb;
    vec3 diffuse = SLight[index].spotLight.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = SLight[index].spotLight.specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = SLight[index].spotLight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
        diffuse = SLight[index].spotLight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = SLight[index].spotLight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], vertex.uv));
    }

    float theta = dot(lightDir, normalize(-SLight[index].spotLight.direction)); 
    float epsilon = SLight[index].spotLight.cutOff - SLight[index].spotLight.outerCutOff;
    float intensity = clamp((theta - SLight[index].spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    float LightDistance = length(LightPos - FragPos2);
    float attenuation = 1.0 / (1.0f + SLight[index].spotLight.linear * LightDistance + SLight[index].spotLight.quadratic * (LightDistance * LightDistance));

    vec3 result = (ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, 10000.0f);
    result *= attenuation;
    return result;
}

vec2 ParallaxMapping( MaterialInfo material, vec2 texCoords, vec3 viewDir)
{ 
    const float heightScale = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.heightScale;
    const float minLayers = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.minLayers;
    const float maxLayers = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.maxLayers;

    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    viewDir.y = -viewDir.y; 
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

vec3 RTXShadow(vec3 LightResult, vec3 LightSpecular, vec3 LightDirection, float LightDistance)
{
//     if(scenedata.Shadowed == 1)
//     {
        float tmin = 0.001;
	    float tmax = LightDistance;
	    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	    shadowed = true;  
	    traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, LightDirection, tmax, 1);
	    if (shadowed) 
        {
            LightResult *= 0.3f;
	    }
        else
        {
           LightResult += LightSpecular;
        }
//    }
//    else
//    {
//           LightResult += LightSpecular;
//    }
    return LightResult;
}