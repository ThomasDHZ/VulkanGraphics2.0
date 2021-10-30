#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "SceneProperties.glsl"
#include "MeshProperties.glsl"
#include "lighting.glsl"
#include "material.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 1) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];
layout(binding = 2) buffer DirectionalLightBuffer{ DirectionalLight directionalLight; } DLight[];
layout(binding = 3) buffer PointLightBuffer { PointLight pointLight; } PLight[];
layout(binding = 4) buffer SpotLightBuffer { SpotLight spotLight; } SLight[];
layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer Material { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap[];

layout(binding = 10) buffer SphereAreaLightBuffer {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float SphereRadius;
    float linear;
    float quadratic;
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
layout(binding = 13) uniform sampler2D ShadowMap[];

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

MaterialInfo material;
mat3 TBN;

vec3 CalcNormalDirLight(vec3 normal, vec2 uv, int index);
vec3 CalcNormalPointLight(vec3 normal, vec2 uv, int index);
vec3 CalcNormalSpotLight(vec3 normal, vec2 uv, int index);
vec3 CalcSphereAreaLight(vec3 normal, vec2 uv, int index);
vec3 CalcTubeAreaLight(vec3 normal, vec2 uv, int index);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec2 offset, int index)
{
    float shadow = 1.0f;
	if ( fragPosLightSpace.z > -1.0 && fragPosLightSpace.z < 1.0 ) 
	{
		float dist = texture( ShadowMap[index], fragPosLightSpace.st + offset ).r;
		if ( fragPosLightSpace.w > 0.0 && dist < fragPosLightSpace.z ) 
		{
			shadow = 0.1f;
		}
	}
    return shadow;
}

float filterPCF(vec4 sc, int index)
{
	ivec2 texDim = textureSize(ShadowMap[index], 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += ShadowCalculation(sc, vec2(dx*x, dy*y), index);
			count++;
		}
	
	}
	return shadowFactor / count;
}

void main() 
{
   material = MaterialList[meshBuffer[Mesh.MeshIndex].meshProperties.MaterialIndex].material;
   vec2 texCoords = TexCoords + meshBuffer[Mesh.MeshIndex].meshProperties.UVOffset;
   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f ||
      texture(TextureMap[material.DiffuseMapID], texCoords).a == 0.0f)
   {
	 discard;
   }
   texCoords *= meshBuffer[Mesh.MeshIndex].meshProperties.UVScale;
   if(meshBuffer[Mesh.MeshIndex].meshProperties.UVFlip.y == 1.0f)
   {
        texCoords.y = 1.0f - texCoords.y;
   }
   if(meshBuffer[Mesh.MeshIndex].meshProperties.UVFlip.x == 1.0f)
   {
        texCoords.x = 1.0f - texCoords.x;
   }
   if(meshBuffer[Mesh.MeshIndex].meshProperties.UVFlip.y == 1.0f)
   {
        texCoords.y = 1.0f - texCoords.y;
   }

   vec3 T = normalize(mat3(meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec3(Tangent));
   vec3 B = normalize(mat3(meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec3(BiTangent));
   vec3 N = normalize(mat3(meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * Normal);
   TBN = transpose(mat3(T, B, N));
   
   vec3 result = vec3(0.0f);
   vec3 normal = Normal;
   vec3 ViewPos  = Mesh.CameraPos;
   vec3 FragPos2  = FragPos;
   vec3 viewDir = normalize(ViewPos - FragPos2);
    if(material.NormalMapID != 0)
    {
        ViewPos  = TBN * Mesh.CameraPos;
        FragPos2  = TBN * FragPos;
    }

    if(material.NormalMapID != 0)
    {
        if(material.DepthMapID != 0)
        {
            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            {
              discard;
            }
        }
        normal = texture(TextureMap[material.NormalMapID], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
     }

   for(int x = 0; x < sceneBuffer.sceneData.DirectionalLightCount; x++)
   {
        result += CalcNormalDirLight(normal, texCoords, x);
   }
//   for(int x = 0; x < sceneBuffer.sceneData.PointLightCount; x++)
//   {
//        result += CalcNormalPointLight(normal, texCoords, x);   
//   }
//   for(int x = 0; x < sceneBuffer.sceneData.SpotLightCount; x++)
//   {
//        result += CalcNormalSpotLight(normal, texCoords, x);   
//   }
//   for(int x = 0; x < sceneBuffer.sceneData.SphereAreaLightCount; x++)
//   {
//       // result += CalcSphereAreaLight(FragPos2, normal, texCoords, x);
//   }
//   for(int x = 0; x < sceneBuffer.sceneData.TubeAreaLightCount; x++)
//   {
//       // result += CalcTubeAreaLight(normal, texCoords, x);   
//   }

    vec3 I = normalize(FragPos2 - ViewPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 Reflection = texture(CubeMap[0], R).rgb;
    vec3 finalMix = mix(result, Reflection, material.Reflectivness);

    outColor = vec4(finalMix, material.Alpha);
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

vec3 CalcNormalDirLight(vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = DLight[index].directionalLight.direction;
    vec3 ViewPos = Mesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[index].directionalLight.direction;
        ViewPos = TBN * Mesh.CameraPos;
        FragPos2 = TBN * FragPos;
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
        ambient = DLight[index].directionalLight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = DLight[index].directionalLight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = DLight[index].directionalLight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float LightDistance = length(LightPos - FragPos2);

    vec4 LightSpace = (LightBiasMatrix *  DLight[index].directionalLight.lightSpaceMatrix * meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec4(FragPos, 1.0);
    float shadow = filterPCF(LightSpace/ LightSpace.w, index);  
    return (ambient + (shadow) * (diffuse + specular));
}

vec3 CalcNormalPointLight(vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = PLight[index].pointLight.position;
    vec3 ViewPos = Mesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * PLight[index].pointLight.position;
        ViewPos = TBN * Mesh.CameraPos;
        FragPos2 = TBN * FragPos;
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
        ambient = PLight[index].pointLight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = PLight[index].pointLight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = PLight[index].pointLight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float LightDistance = length(LightPos - FragPos2);
    float attenuation = 1.0 / (1.0f + PLight[index].pointLight.linear * LightDistance + PLight[index].pointLight.quadratic * (LightDistance * LightDistance));

        const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

    vec4 LightSpace = (biasMat * sceneBuffer.sceneData.lightSpaceMatrix * meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec4(FragPos, 1.0);
   float shadow = filterPCF(LightSpace/ LightSpace.w, index);  
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation;
}

vec3 CalcNormalSpotLight(vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = SLight[index].spotLight.position;
    vec3 ViewPos = Mesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * SLight[index].spotLight.position;
        ViewPos = TBN * Mesh.CameraPos;
        FragPos2 = TBN * FragPos;
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
        ambient = SLight[index].spotLight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = SLight[index].spotLight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = SLight[index].spotLight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float theta = dot(lightDir, normalize(-SLight[index].spotLight.direction)); 
    float epsilon = SLight[index].spotLight.cutOff - SLight[index].spotLight.outerCutOff;
    float intensity = clamp((theta - SLight[index].spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    float LightDistance = length(LightPos - FragPos2);
    float attenuation = 1.0 / (1.0f + SLight[index].spotLight.linear * LightDistance + SLight[index].spotLight.quadratic * (LightDistance * LightDistance));

    return (ambient + diffuse + specular) * attenuation * intensity;
}

vec3 CalcSphereAreaLight(vec3 normal, vec2 uv, int index)
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

    float LightDistance = length((LightPos + (SphereLight[index].SphereRadius/2)) - FragPos2);
    float attenuation = 1.0 / (1.0f + SphereLight[index].linear * LightDistance + SphereLight[index].quadratic * (LightDistance * LightDistance));

    return (ambient + diffuse + specular) * attenuation;
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

vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir)
{
    const float heightScale = meshBuffer[Mesh.MeshIndex].meshProperties.heightScale;
    const float minLayers = meshBuffer[Mesh.MeshIndex].meshProperties.minLayers;
    const float maxLayers = meshBuffer[Mesh.MeshIndex].meshProperties.maxLayers;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    viewDir.y = -viewDir.y;
    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2  currentTexCoords = texCoords;
    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}