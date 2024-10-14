#ifndef BINDLESS_TEXTURE_MANAGER_H
#define BINDLESS_TEXTURE_MANAGER_H


#include "Neko.h"

#include <vector>


class Texture;


class BindlessTextureManager
{
public:
	BindlessTextureManager();
	~BindlessTextureManager();

	u32 AddTexture(Texture* texture);

	void UpdateSSBO();
	void Bind(u32 index) const;

private:
	std::vector<u64> m_Handles;
	std::vector<Texture*> m_Textures;

	u32 m_SSBO;
};


#endif // !BINDLESS_TEXTURE_MANAGER_H
