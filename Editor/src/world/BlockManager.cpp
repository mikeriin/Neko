#include "world/BlockManager.h"


BlockManager* BlockManager::m_Instance = nullptr;
std::map<BlockType, u32> BlockManager::m_BlockTextureMap;


BlockManager* BlockManager::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance = new BlockManager();
	}

	return m_Instance;
}

void BlockManager::MapTexture(BlockType blockIndex, u32 textureHandleIndex)
{
	m_BlockTextureMap.insert(std::make_pair(blockIndex, textureHandleIndex));
}

u32 BlockManager::GetTextureHandleIndex(BlockType blockIndex) const
{
	if (auto it = m_BlockTextureMap.find(blockIndex); it != m_BlockTextureMap.end())
	{
		return it->second;
	}
	return 0;
}
