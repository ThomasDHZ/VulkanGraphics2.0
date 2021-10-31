#include "Material.glsl"

struct MeshProperties
{
	MaterialInfo material;
	mat4 ModelTransform;
	mat4 MeshTransform;
	vec2 UVOffset;
	vec2 UVScale;
	vec2 UVFlip;
	uint MaterialIndex;
	float heightScale;
	float minLayers;
	float maxLayers;
};