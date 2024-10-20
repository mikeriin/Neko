#version 460 core

#extension GL_ARB_bindless_texture : require


// layout(location = 0) in vec3 aPos;
layout(location = 0) in uint aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aTexIndex;


out vec3 vertexNormal;
out vec2 vertexUV;
flat out uint textureIndex;


uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;


uniform vec3 chunkWorldPos;


uvec3 decodeCoords(uint coords)
{
	uint x = (coords >> 16) & 0xFF;
	uint y = (coords >> 8) & 0xFF;
	uint z = coords & 0xFF;
	return uvec3(x, y, z);
}


void main()
{
	vertexNormal = aNorm;
	vertexUV = aUV;
	textureIndex = aTexIndex;

	vec3 vertexPos = chunkWorldPos + vec3(decodeCoords(aPos));
	gl_Position = projection * view * transform * vec4(vertexPos, 1.0);
}