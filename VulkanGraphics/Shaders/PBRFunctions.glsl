PBRMaterial PBRMaterialBuilder(MaterialInfo materialInfo, vec2 uv)
{
    PBRMaterial material;

    material.Albedo = materialInfo.Albedo;
    if (materialInfo.AlbedoMapID != 0)
    {
        material.Albedo = texture(TextureMap[materialInfo.AlbedoMapID], uv).rgb;
    }

    if (materialInfo.NormalMapID != 0)
    {
        material.Normal = texture(TextureMap[materialInfo.NormalMapID], uv).rgb;
    }

    if (materialInfo.DepthMapID != 0)
    {
        material.Depth = texture(TextureMap[materialInfo.DepthMapID], uv).rgb;
    }

    if (materialInfo.EmissionMapID != 0)
    {
        material.Emission = texture(TextureMap[materialInfo.EmissionMapID], uv).rgb;
    }

    material.Metallic = materialInfo.Matallic;
    if (materialInfo.MatallicMapID != 0)
    {
        material.Metallic = texture(TextureMap[materialInfo.MatallicMapID], uv).r;
    }

    material.Roughness = materialInfo.Roughness;
    if (materialInfo.RoughnessMapID != 0)
    {
        material.Roughness = texture(TextureMap[materialInfo.RoughnessMapID], uv).r;
    }

    material.AmbientOcclusion = materialInfo.AmbientOcclusion;
    if (materialInfo.AOMapID != 0)
    {
        material.AmbientOcclusion = texture(TextureMap[materialInfo.AOMapID], uv).r;
    }

    material.Alpha = materialInfo.Alpha;
    if (materialInfo.AlphaMapID != 0)
    {
        material.Alpha = texture(TextureMap[materialInfo.AlphaMapID], uv).r;
    }

    return material;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 CalcDirectionalLight(PBRMaterial material, vec3 F0, vec3 V, vec3 N)
{

    vec3 Lo = vec3(0.0);
    for (int x = 0; x < scenedata.DirectionalLightCount; x++)
    {
        vec3 L = normalize(-DLight[x].direction);
        vec3 H = normalize(V + L);

        vec3 radiance = DLight[x].diffuse;

        float NDF = DistributionGGX(N, H, material.Roughness);
        float G = GeometrySmith(N, V, L, material.Roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.Metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.Albedo / PI + specular) * radiance * NdotL;
    }
    return Lo;
}

vec3 CalcPointLight(PBRMaterial material, vec3 Pos, vec3 F0, vec3 V, vec3 N)
{
    vec3 Lo = vec3(0.0);
    for (int x = 0; x < scenedata.PointLightCount; x++)
    {
        vec3 L = normalize(PLight[x].position - Pos);
        vec3 H = normalize(V + L);

        float distance = length(PLight[x].position - Pos);
        float attenuation = 1.0 / (PLight[x].constant + PLight[x].linear * distance + PLight[x].quadratic * (distance * distance));
        vec3 radiance = PLight[x].diffuse * attenuation;

        float NDF = DistributionGGX(N, H, material.Roughness);
        float G = GeometrySmith(N, V, L, material.Roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.Metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.Albedo / PI + specular) * radiance * NdotL;
    }
    return Lo;
}

vec3 CalcSpotLight(PBRMaterial material, vec3 Pos, vec3 F0, vec3 V, vec3 N)
{
    vec3 Lo = vec3(0.0);
    for (int x = 0; x < scenedata.SpotLightCount; x++)
    {
        vec3 L = normalize(SLight[x].position - Pos);
        vec3 H = normalize(V + L);

        float distance = length(SLight[x].position - Pos);
        float attenuation = 1.0 / (SLight[x].constant + SLight[x].linear * distance + SLight[x].quadratic * (distance * distance));
        float theta = dot(SLight[x].direction, normalize(-SLight[x].direction));
        float epsilon = SLight[x].cutOff - SLight[x].outerCutOff;
        float intensity = clamp((theta - SLight[x].outerCutOff) / epsilon, 0.0, 1.0);
        vec3 radiance = SLight[x].diffuse * attenuation * intensity;

        float NDF = DistributionGGX(N, H, material.Roughness);
        float G = GeometrySmith(N, V, L, material.Roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.Metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.Albedo / PI + specular) * radiance * NdotL;
    }
    return Lo;
}