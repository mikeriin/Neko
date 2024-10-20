#version 460 core


layout(location = 0) in uint aPos;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;


uvec3 decodeCoords(uint coords)
{
	uint x = (coords >> 8) & 0xF;
	uint y = (coords >> 4) & 0xF;
	uint z = coords & 0xF;
	return uvec3(x, y, z);
}


void main()
{
	vec3 vertexPos = vec3(decodeCoords(aPos));
	gl_Position = projection * view * transform * vec4(vertexPos, 1.0);
}