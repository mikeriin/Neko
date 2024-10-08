#ifndef TEXTURE_H
#define TEXTURE_H


#include <vector>


#include "Neko.h"


#include "glad/glad.h"


class Texture
{
public:
	Texture() = delete;
	Texture(const char* textureFilePath);

	~Texture();
	
	inline void BindUnit(u32 unit) { m_TextureUnit = unit; glBindTextureUnit(unit, m_TextureId); }
	inline void Unind() const { glBindTextureUnit(m_TextureUnit, 0); }

	inline u32 GetID() const { return m_TextureId; }

	static void SetPixelated(bool isPixelated);

	static std::vector<u32> Textures;
	static std::vector<GLuint64> TextureHandles;

private:
	u32 m_TextureId{};
	u32 m_TextureUnit{};

	static bool sm_IsPixelated;
};


#endif // !TEXTURE_H
