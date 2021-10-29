struct SceneProperties
{
    mat4 lightSpaceMatrix;
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    uint SphereAreaLightCount;
    uint TubeAreaLightCount;
    uint RectangleAreaLightCount;
    float timer;
    int Shadowed;
    int temp;
};