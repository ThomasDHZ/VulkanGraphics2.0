#define VertexTransformCount 300

struct VertexBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightSpaceMatrix;
    mat4 BoneTransform[VertexTransformCount];
};