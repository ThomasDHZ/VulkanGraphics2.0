void LogVec3(string Tag, int VertexIndex, vec3 numbers)
{
	debugPrintfEXT(Tag, "= %i, %f, %f, %f, %f \n", gl_GlobalInvocationID.x, numbers.x, numbers.y, numbers.z, numbers.w);
}