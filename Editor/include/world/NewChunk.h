#ifndef NEW_CHUNK_H
#define NEW_CHUNK_H


#include "Neko.h"

#include <unordered_map>
#include <tuple>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>

#include <glm/glm.hpp>
#include <noise/PerlinNoise.hpp>

#include "BlockManager.h"
#include "utils/BlockType.h"


struct Vertex;
class Buffer;
class VertexArray;
class BlockManager;
class Shader;


const u32 CHUNK_SIZE = 16;
const u32 MAX_GENERATION_HEIGHT = 96;
const u32 PLAIN_HEIGHT = 32;


struct ChunkSettings
{
	u32 Seed								= 123456u;
	glm::vec3 Position			= glm::vec3{ 0, 0, 0 };
};


class NewChunk
{
public:
	NewChunk(const ChunkSettings& settings);
	~NewChunk();

	void Generate();
	inline bool IsGenerated() const { return m_IsGenerationFinished; }
	void Render(Shader& sh);
	bool IsChunkEmpty() const { return m_Indices.size() == 0; }

	inline const ChunkSettings& GetSettings() const { return m_Settings; }

private:

	ChunkSettings m_Settings;

	std::thread m_GenerationThread;
	std::mutex m_Mutex;
	std::atomic<bool> m_IsGenerationFinished;

	std::vector<BlockType> m_Datas;

	std::vector<Vertex> m_Vertices;
	std::vector<u32> m_Indices;

	Buffer* m_VBO;
	Buffer* m_EBO;
	VertexArray* m_VAO;

	BlockManager* m_BlockManagerInstance;
};



#endif // !NEW_CHUNK_H
