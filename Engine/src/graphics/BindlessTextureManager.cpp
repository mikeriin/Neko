#include "graphics/BindlessTextureManager.h"


#include <iostream>

#include <glad/glad.h>

#include "graphics/Texture.h"


BindlessTextureManager::BindlessTextureManager()
	: m_SSBO(0)
{
}

BindlessTextureManager::~BindlessTextureManager()
{
	for (u64 handle : m_Handles) glMakeTextureHandleNonResidentARB(handle);
	for (Texture* texture : m_Textures) delete texture;
	glDeleteBuffers(1, &m_SSBO);
}

u32 BindlessTextureManager::AddTexture(Texture* texture)
{
	u64 handle = glGetTextureHandleARB(texture->GetID());
	if (!handle)
	{
		std::cerr << "BINDLESS_TEXTURE_MANAGER::ERROR::glGetTextureHandleARB\n";
		return 0;
	}

	glMakeTextureHandleResidentARB(handle);
	m_Handles.push_back(handle);
	m_Textures.push_back(texture);

	return static_cast<u32>(m_Handles.size() - 1);
}

void BindlessTextureManager::UpdateSSBO()
{
	if (m_Handles.size() == 0) return;

	if (!m_SSBO) // first time using this ssbo, init
	{
		glCreateBuffers(1, &m_SSBO);
	}

	glNamedBufferData(m_SSBO, sizeof(u64) * m_Handles.size(), m_Handles.data(), GL_DYNAMIC_READ);
}

void BindlessTextureManager::Bind(u32 index) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_SSBO);
}
