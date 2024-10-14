#version 460 core

#extension GL_ARB_bindless_texture : require


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aTexIndex;


out vec3 vertexNormal;
out vec2 vertexUV;
flat out uint textureIndex;


uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	vertexNormal = aNorm;
	vertexUV = aUV;
	textureIndex = aTexIndex;
	gl_Position = projection * view * transform * vec4(aPos, 1.0);
}