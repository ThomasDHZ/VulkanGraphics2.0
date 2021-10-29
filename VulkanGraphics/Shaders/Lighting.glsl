struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    mat4 lightSpaceMatrix;
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
    mat4 lightSpaceMatrix;
};

struct SpotLight 
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
    mat4 lightSpaceMatrix;
};

struct SphereAreaLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float SphereRadius;
    float linear;
    float quadratic;
    mat4 lightSpaceMatrix;
};

struct TubeAreaLight
{
    vec3 StartPos;
    vec3 EndPos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float TubeRadius;
    mat4 lightSpaceMatrix;
};

struct RectangleAreaLight
{
    vec3 VertexPos1;
    vec3 VertexPos2;
    vec3 VertexPos3;
    vec3 VertexPos4;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    mat4 lightSpaceMatrix;
};

const mat4 LightBiasMatrix = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0);