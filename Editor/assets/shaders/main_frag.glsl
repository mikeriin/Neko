#version 460 core


#extension GL_ARB_bindless_texture : require



in vec3 vertexNormal;
in vec2 vertexUV;
flat in int texIndex;


out vec4 FragColor;


uniform vec3 colorMult;


uniform sampler2D texBlock; //0
uniform sampler2D texOverlay; //1
uniform sampler2D texTop; //2



void main()
{
	vec3 colorCorrection = colorMult;
	if (colorMult == vec3(0.0)) colorCorrection = vec3(1.0);

	float light = 1.0;

	vec4 blockColor = texture(texBlock, vertexUV);
	vec4 overlayColor = texture(texOverlay, vertexUV) * vec4(colorCorrection, 1.0);
	vec4 topColor = texture(texTop, vertexUV) * vec4(colorCorrection, 1.0);

	vec4 finalColor = blockColor;
	
	if (vertexNormal.x != 0 || vertexNormal.z != 0) // overlay
	{
		finalColor = mix(finalColor, overlayColor, overlayColor.a);
		if (vertexNormal.x < 0) light = 0.6;
		else light = 0.9;
		if (vertexNormal.z < 0) light = 0.8;
		else light = 0.7;
	}
	else if (vertexNormal.y != 0) // top
	{
		if (vertexNormal.y > 0) finalColor = topColor;
		else light = 0.5;
	}

	FragColor = vec4(finalColor.rgb * light, finalColor.a);
}