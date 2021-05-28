
vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = DLight[index].direction;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[index].direction;
        ViewPos = TBN * ConstMesh.CameraPos;
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

    return vec3(specular);
}

vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, vec2 uv, int index)
{
    vec3 LightPos = PLight[index].position;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * PLight[index].position;
        ViewPos = TBN * ConstMesh.CameraPos;
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

    float distance = length(LightPos - FragPos2);
    float attenuation = 1.0 / (PLight[index].constant + PLight[index].linear * distance + PLight[index].quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcNormalSpotLight(vec3 FragPos, SpotLight light, vec3 normal, vec2 uv)
{
    vec3 LightPos = light.position;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * light.position;
        ViewPos = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * FragPos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    vec3 lightDir = normalize(light.position - FragPos2);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    float distance = length(light.position - FragPos2);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * material.Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
//
//vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir)
//{
//    const float heightScale = meshProperties[ConstMesh.MeshIndex].heightScale;
//    const float minLayers = meshProperties[ConstMesh.MeshIndex].minLayers;
//    const float maxLayers = meshProperties[ConstMesh.MeshIndex].maxLayers;
//    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
//    // calculate the size of each layer
//    float layerDepth = 1.0 / numLayers;
//    // depth of current layer
//    float currentLayerDepth = 0.0;
//    // the amount to shift the texture coordinates per layer (from vector P)
//    viewDir.y = -viewDir.y;
//    vec2 P = viewDir.xy / viewDir.z * heightScale;
//    vec2 deltaTexCoords = P / numLayers;
//
//    // get initial values
//    vec2  currentTexCoords = texCoords;
//    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
//
//    while (currentLayerDepth < currentDepthMapValue)
//    {
//        // shift texture coordinates along direction of P
//        currentTexCoords -= deltaTexCoords;
//        // get depthmap value at current texture coordinates
//        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
//        // get depth of next layer
//        currentLayerDepth += layerDepth;
//    }
//
//    // get texture coordinates before collision (reverse operations)
//    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
//
//    // get depth after and before collision for linear interpolation
//    float afterDepth = currentDepthMapValue - currentLayerDepth;
//    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;
//
//    // interpolation of texture coordinates
//    float weight = afterDepth / (afterDepth - beforeDepth);
//    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
//
//    return finalTexCoords;
//}