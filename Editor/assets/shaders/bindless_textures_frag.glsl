#version 460 core


#extension GL_ARB_bindless_texture : require


layout(binding = 0, std430) readonly buffer textureHandles
{
	sampler2D bindlessTextures[];
};



in vec3 vertexNormal;
in vec2 vertexUV;


out vec4 FragColor;



void main()
{
	FragColor = texture(bindlessTextures[0], vertexUV);
}