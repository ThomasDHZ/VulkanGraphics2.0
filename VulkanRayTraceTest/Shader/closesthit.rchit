#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

#include "Vertex.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 1) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

layout(binding = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform UBO 
{
	DirectionalLight dlight;
	PointLight plight[5];
	SpotLight sLight;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
	float timer;
    int temp;
} ubo;
layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
    uint MaterialIndex;
} meshProperties[];
layout(binding = 4, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 5) buffer Indices { uint i[]; } indices[];
layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D TextureMap[];
layout(binding = 9) uniform sampler3D Texture3DMap[];

Vertex BuildVertexInfo();
vec3 CalcNormalDirLight(Vertex vertex, MaterialInfo material, mat3 TBN, vec3 normal, vec2 uv);
vec3 CalcNormalPointLight(Vertex vertex, MaterialInfo material, mat3 TBN, PointLight light, vec3 normal, vec2 uv);
vec3 CalcNormalSpotLight(Vertex vertex, MaterialInfo material, mat3 TBN, SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 uv);
vec3 CalcDirLight(Vertex vertex, MaterialInfo material, DirectionalLight light, vec2 uv);
vec3 CalcPointLight(Vertex vertex, MaterialInfo material, PointLight light, vec2 uv);
vec3 CalcSpotLight(Vertex vertex, MaterialInfo material, SpotLight light, vec2 uv);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
    const Vertex vertex = BuildVertexInfo();
    const MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;

    const vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
    const vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
    const vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
    const mat3 TBN = transpose(mat3(T, B, N));
   
       const vec3 TangentViewPos  = TBN * ubo.viewPos;
    const vec3 TangentFragPos  = TBN * vertex.pos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
   vec2 uv = vertex.uv;

    vec3 normal = vertex.normal;
    if(material.NormalMapID != 0)
    {
        
        normal = texture(TextureMap[material.NormalMapID], uv).rgb;
        normal = normalize(normal * 2.0 - 1.0);

        hitValue = CalcNormalDirLight(vertex, material, TBN, normal, uv);
        for(int x = 0; x < 5; x++)
        {
            hitValue += CalcNormalPointLight(vertex, material, TBN, ubo.plight[x], normal, uv);   
        }
      //  hitValue +=  CalcNormalSpotLight(vertex, material, TBN, ubo.sLight, normal, vertex.pos, viewDir, uv);
    }
    else
    {
        hitValue = CalcDirLight(vertex, material, ubo.dlight, uv);
        for(int x = 0; x < 5; x++)
        {
            hitValue += CalcPointLight(vertex, material, ubo.plight[x], uv);   
        }
      //  hitValue +=  CalcSpotLight(vertex, material, TBN, ubo.sLight, normal, vertex.pos, viewDir, uv);
    }
}

Vertex BuildVertexInfo()
{
    Vertex vertex;
    const ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	const Vertex v0 = vertices[gl_InstanceCustomIndexEXT].v[index.x];
	const Vertex v1 = vertices[gl_InstanceCustomIndexEXT].v[index.y];
	const Vertex v2 = vertices[gl_InstanceCustomIndexEXT].v[index.z];

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);

	vertex.pos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	vertex.pos = vec3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform * vec4(vertex.pos, 1.0));

    vertex.normal = v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z;
    vertex.normal = mat3(transpose(inverse(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform))) * vertex.normal;  

	vertex.uv = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;
    vertex.uv += meshProperties[gl_InstanceCustomIndexEXT].UVOffset;

	vertex.tangent = v0.tangent * barycentricCoords.x + v1.tangent * barycentricCoords.y + v2.tangent * barycentricCoords.z;
	vertex.BiTangant = v0.BiTangant * barycentricCoords.x + v1.BiTangant * barycentricCoords.y + v2.BiTangant * barycentricCoords.z;

    const vec3 color = v0.Color.xyz * barycentricCoords.x + v1.Color.xyz * barycentricCoords.y + v2.Color.xyz * barycentricCoords.z;
	vertex.Color = vec4(color, 1.0f);

    return vertex;
}

vec3 CalcNormalDirLight(Vertex vertex, MaterialInfo material, mat3 TBN, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * ubo.dlight.direction;
    const vec3 TangentViewPos  = TBN * ubo.viewPos;
    const vec3 TangentFragPos  = TBN * vertex.pos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 lightDir = normalize(-TangentLightPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = ubo.dlight.ambient * vertex.Color.rgb;
    vec3 diffuse = ubo.dlight.diffuse * diff * vertex.Color.rgb;
    vec3 specular = ubo.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = ubo.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = ubo.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = ubo.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    
    vec3 result = vec3(ambient + diffuse + specular);

//    float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 1);
//	if (shadowed) {
//		result = ambient;
//	}

    return result;
}

vec3 CalcNormalPointLight(Vertex vertex, MaterialInfo material, mat3 TBN, PointLight light, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * light.position;
    const vec3 TangentViewPos  = TBN * ubo.viewPos;
    const vec3 TangentFragPos  = TBN * vertex.pos;  
 
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = light.ambient * vertex.Color.rgb;
    vec3 diffuse = light.diffuse * diff * vertex.Color.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float distance = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular);
//
//    float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 1);
//	if (shadowed) {
//		result = ambient;
//	}

    return result;
}

vec3 CalcNormalSpotLight(Vertex vertex, MaterialInfo material, mat3 TBN, SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 uv)
{
    const vec3 TangentLightPos = TBN * light.position;
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vertex.Color.rgb;
    vec3 diffuse = light.diffuse * diff * vertex.Color.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    vec3 result = (ambient + diffuse + specular);

//    float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 1);
//	if (shadowed) {
//			result *= 0.3f;
//	}

    return result;
}

vec3 CalcDirLight(Vertex vertex, MaterialInfo material, DirectionalLight light, vec2 uv)
{
    vec3 viewDir = normalize(ubo.viewPos - vertex.pos);
    vec3 lightDir = normalize(-ubo.dlight.direction);

    float diff = max(dot(vertex.normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, vertex.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    vec3 ambient = ubo.dlight.ambient * vertex.Color.rgb;
    vec3 diffuse = ubo.dlight.diffuse * diff * vertex.Color.rgb;
    vec3 specular = ubo.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = ubo.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
        diffuse = ubo.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = ubo.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    vec3 result = vec3(ambient + diffuse + specular);

//    float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 1);
//	if (shadowed) {
//		result *= 0.3f;
//	}

    return result;
}

vec3 CalcPointLight(Vertex vertex, MaterialInfo material, PointLight light, vec2 uv)
{
    vec3 viewDir = normalize(ubo.viewPos - vertex.pos);
    vec3 lightDir = normalize(light.position - vertex.pos);

    float diff = max(dot(vertex.normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, vertex.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    float distance = length(light.position - vertex.pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * vertex.Color.rgb;
    vec3 diffuse = light.diffuse * diff * vertex.Color.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;


    vec3 result = vec3(ambient + diffuse + specular);

//    float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 1);
//	if (shadowed) {
//		result *= 0.3f;
//	}

    return result;
}

vec3 CalcSpotLight(Vertex vertex, MaterialInfo material, SpotLight light, vec2 uv)
{
    vec3 viewDir = normalize(ubo.viewPos - vertex.pos);
    vec3 lightDir = normalize(light.position - vertex.pos);

    float diff = max(dot(vertex.normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, vertex.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    float distance = length(light.position - vertex.pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vertex.Color.rgb;
    vec3 diffuse = light.diffuse * diff * vertex.Color.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;



    vec3 result = vec3(ambient + diffuse + specular);

//    float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 1);
//	if (shadowed) {
//		result *= 0.3f;
//	}

    return result;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float heightScale = 0.1;
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(TextureMap[3], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(TextureMap[3], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[3], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}