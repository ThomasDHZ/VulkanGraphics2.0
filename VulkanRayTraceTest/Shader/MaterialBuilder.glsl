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