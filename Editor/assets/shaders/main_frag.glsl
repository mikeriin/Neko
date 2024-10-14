#version 460 core


const uint ATLAS_SIZE = 16;


in vec3 vertexNormal;
in vec2 vertexUV;
in float textureIndex;


out vec4 FragColor;


uniform vec3 colorMult;


uniform sampler2D textures;



void main()
{
	vec3 colorCorrection = colorMult;
	if (colorMult == vec3(0.0)) colorCorrection = vec3(1.0);

	float light = 1.0;

	vec2 atlasVertexUV = vertexUV / ATLAS_SIZE;
	atlasVertexUV += vec2(mod(textureIndex, ATLAS_SIZE), floor(textureIndex / ATLAS_SIZE)) / ATLAS_SIZE;
	vec4 blockColor = texture(textures, atlasVertexUV);
	
	if (vertexNormal.x != 0) light = 0.65;
	else if (vertexNormal.z != 0) light = 0.85;
	else if (vertexNormal.y != 0) 
	{
		if (vertexNormal.y > 0) light = 1.0;
		else light = 0.5;
	}

	// FragColor = vec4(finalColor.rgb * light, finalColor.a);
	// if (textureIndex == 0) FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	// else if (textureIndex == 1) FragColor = vec4(0.0, 0.0, 1.0, 1.0);
	// else if (textureIndex == 2) FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else FragColor = texture(textures, atlasVertexUV);

	FragColor = vec4(blockColor.rgb * light, blockColor.a);
}