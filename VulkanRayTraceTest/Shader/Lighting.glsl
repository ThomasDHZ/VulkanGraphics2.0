struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

vec3 CalcAmbient(vec3 LightColor, vec3 MaterialColor)
{
    return LightColor * MaterialColor;
}

vec3 CalcDiffuse(vec3 LightDirection, vec3 LightColor, vec3 MaterialColor)
{
    vec3 lightDir = normalize(-LightDirection);
    float diff = max(dot(LightDirection, lightDir), 0.0);
    return LightColor * diff * MaterialColor;
}

vec3 CalcSpecular(vec3 LightDirection, vec3 SpecularColor, vec3 MaterialColor, vec3 CameraPos, vec3 Normal, float Shiniess)
{
    vec3 halfwayDir = normalize(LightDirection + CameraPos);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shiniess);
    return SpecularColor * spec * SpecularColor;
}