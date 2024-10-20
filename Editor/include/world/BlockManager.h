#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H


#include "Neko.h"

#include <map>

#include "utils/BlockType.h"


class BlockManager
{
public:
	static BlockManager* GetInstance();

	void MapTexture(BlockType blockIndex, u32 textureHandleIndex);
	u32 GetTextureHandleIndex(BlockType blockIndex) const;

protected:
	BlockManager() = default;

private:
	BlockManager(BlockManager& other) = delete;
	BlockManager& operator=(const BlockManager& other) = delete;

	static BlockManager* m_Instance;
	static std::map<BlockType, u32> m_BlockTextureMap;
};


#endif // !BLOCK_MANAGER_H

