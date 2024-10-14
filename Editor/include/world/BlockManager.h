#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H


#include "Neko.h"

#include <map>


enum Block
{
	Debug,
	Dirt,
	GrassBlock,
	GrassBlockSide,
	Stone,
	Snow,
	GoldBlock,
	Bedrock,
};


class BlockManager
{
public:
	static BlockManager* GetInstance();

	void MapTexture(Block blockIndex, u32 textureHandleIndex);
	u32 GetTextureHandleIndex(Block blockIndex) const;

protected:
	BlockManager() = default;

private:
	BlockManager(BlockManager& other) = delete;
	BlockManager& operator=(const BlockManager& other) = delete;

	static BlockManager* m_Instance;
	static std::map<Block, u32> m_BlockTextureMap;
};


#endif // !BLOCK_MANAGER_H

