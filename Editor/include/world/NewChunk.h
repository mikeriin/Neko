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


struct Vertex;
class Buffer;
class VertexArray;
class BlockManager;


struct tuple3_hash
{
	std::size_t operator()(const std::tuple<i32, i32, i32>& key) const {
		return std::hash<int>()(std::get<0>(key)) ^ (std::hash<int>()(std::get<1>(key)) << 1) ^ (std::hash<int>()(std::get<2>(key)) << 2);
	}
};


struct tuple3_equal
{
	bool operator()(const std::tuple<i32, i32, i32>& lhs, const std::tuple<i32, i32, i32>& rhs) const {
		return std::get<0>(lhs) == std::get<0>(rhs)
			&& std::get<1>(lhs) == std::get<1>(rhs)
			&& std::get<2>(lhs) == std::get<2>(rhs);
	}
};


struct ChunkSettings
{
	u32 Seed								= 123456u;
	glm::ivec3 Position			= glm::ivec3{ 0, 0, 0 };
	u32 Size								= 16;
	u32 MaxHeight						= 192;
};


class NewChunk
{
public:
	NewChunk(const ChunkSettings& settings);
	~NewChunk();

	void Generate();
	inline bool IsGenerated() const { return m_IsGenerationFinished; }
	void Render();
	bool IsChunkEmpty() const { return m_Indices.size() == 0; }

	inline const ChunkSettings& GetSettings() const { return m_Settings; }

private:

	ChunkSettings m_Settings;

	std::thread m_GenerationThread;
	std::mutex m_Mutex;
	std::atomic<bool> m_IsGenerationFinished;

	std::unordered_map<std::tuple<i32, i32, i32>, Block, tuple3_hash, tuple3_equal> m_Datas;
	std::vector<Vertex> m_Vertices;
	std::vector<u32> m_Indices;

	Buffer* m_VBO;
	Buffer* m_EBO;
	VertexArray* m_VAO;

	BlockManager* m_BlockManagerInstance;
};



#endif // !NEW_CHUNK_H
