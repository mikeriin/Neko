#include "graphics/Texture.h"


#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


std::vector<u32> Texture::Textures = {};
std::vector<u64> Texture::TextureHandles = {};
bool Texture::sm_IsPixelated = false;


Texture::Texture(const char* textureFilePath)
{
	u32 textureID;

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

	glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, sm_IsPixelated ? GL_NEAREST : GL_LINEAR);
	glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, sm_IsPixelated ? GL_NEAREST : GL_LINEAR);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pixels = stbi_load(textureFilePath, &width, &height, &channels, 0);
	if (pixels)
	{
		u32 internalFormat{};
		u32 externalFormat{};
		switch (channels)
		{
		case 1:
		{
			// convert grayscale img to rgb
			unsigned char* rgbPixels = new unsigned char[width * height * 3];
			for (int i = 0; i < width * height; ++i)
			{
				rgbPixels[i * 3] = pixels[i];     // Red
				rgbPixels[i * 3 + 1] = pixels[i]; // Green
				rgbPixels[i * 3 + 2] = pixels[i]; // Blue
			}

			stbi_image_free(pixels);
			pixels = rgbPixels;
			internalFormat = GL_RGB8;
			externalFormat = GL_RGB;
			break;
		}

		case 3:
			internalFormat = GL_RGB8;
			externalFormat = GL_RGB;
			break;

		case 4:
			internalFormat = GL_RGBA8;
			externalFormat = GL_RGBA;
			break;

		default:
			std::cerr << "TEXTURE::ERROR::Unsupported texture format\n";
			stbi_image_free(pixels);
			return;
		}

		glTextureStorage2D(textureID, 1, internalFormat, width, height);
		glTextureSubImage2D(textureID, 0, 0, 0, width, height, externalFormat, GL_UNSIGNED_BYTE, pixels);

		u64 handle = glGetTextureHandleARB(textureID);
		if (!handle) std::cerr << "TEXTURE::ERROR::glGetTextureHandleARB: " << glGetError() << "\n";
		glMakeTextureHandleResidentARB(handle);

		Textures.push_back(textureID);
		TextureHandles.push_back(handle);
	}
	else
	{
		std::cerr << "TEXURE::ERROR::stbi_load:" << textureFilePath << "\n";
		return;
	}
	stbi_image_free(pixels);
}

Texture::~Texture()
{
	//glDeleteTextures(1, &m_TextureId);
}


void Texture::SetPixelated(bool isPixelated)
{
	sm_IsPixelated = isPixelated;
}
