#version 460 core

#extension GL_ARB_bindless_texture : require


layout(binding = 0, std430) readonly buffer textures
{
	sampler2D bindlessTextures[];
};


in vec3 vertexNormal;
in vec2 vertexUV;
flat in uint textureIndex;


out vec4 FragColor;



void main()
{
	float light = 1.0;

	vec4 blockColor = texture(bindlessTextures[textureIndex], vertexUV);
	
	if (vertexNormal.x != 0) light = 0.65;
	else if (vertexNormal.z != 0) light = 0.85;
	else if (vertexNormal.y != 0) 
	{
		if (vertexNormal.y > 0) light = 1.0;
		else light = 0.5;
	}

	FragColor = vec4(blockColor.rgb * light, blockColor.a);
}