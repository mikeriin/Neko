#include "world/NewChunk.h"


#include <cmath>
#include <algorithm>

#include <glad/glad.h>

#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"
#include "graphics/Shader.h"

#include "Block.h"
#include "world/BlockManager.h"


static f64 smoothstep(f64 edge0, f64 edge1, f64 x)
{
	// Scale, bias and saturate x to 0..1 range
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}

static f64 lerp(f64 a, f64 b, f64 f)
{
	return a + f * (b - a);
}



NewChunk::NewChunk(const ChunkSettings& settings)
	: m_Settings(settings)
	, m_IsGenerationFinished(false)
	, m_VBO(nullptr)
	, m_EBO(nullptr)
	, m_VAO(nullptr)
	, m_BlockManagerInstance(BlockManager::GetInstance())
	, m_Datas((CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2), BlockType::Air)
{

}

NewChunk::~NewChunk()
{
	if (m_VBO) delete m_VBO;
	if (m_EBO) delete m_EBO;
	if (m_VAO) delete m_VAO;

	m_Datas.clear();
}

void NewChunk::Generate()
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	const siv::PerlinNoise::seed_type seed = m_Settings.Seed;
	const siv::PerlinNoise perlin{ seed };

	glm::ivec3 worldPosition{
		m_Settings.Position.x * CHUNK_SIZE,
		m_Settings.Position.y * CHUNK_SIZE,
		m_Settings.Position.z * CHUNK_SIZE,
	};

	i32 changes = 0;
	for (i32 z = worldPosition.z; z <= static_cast<i32>(worldPosition.z + CHUNK_SIZE + 1); z++)
	{
		for (i32 x = worldPosition.x; x <= static_cast<i32>(worldPosition.x + CHUNK_SIZE + 1); x++)
		{
			f64 biomeNoise = perlin.noise2D_01((f64)(x - 1) * 0.01, (f64)(z - 1) * 0.01);

			f64 plainNoise = perlin.octave2D_01((f64)(x - 1) * 0.015, (f64)(z - 1) * 0.015, 4, 0.2);
			f64 mountainNoise = perlin.octave2D_01((f64)(x - 1) * 0.025, (f64)(z - 1) * 0.025, 8, 0.4);

			f64 smoothedNoise = smoothstep(0.3, 0.75, biomeNoise);
			i32 noise = static_cast<i32>(lerp(plainNoise * PLAIN_HEIGHT, mountainNoise * MAX_GENERATION_HEIGHT, smoothedNoise));


			for (i32 y = worldPosition.y; y <= static_cast<i32>(worldPosition.y + CHUNK_SIZE + 1); y++)
			{
				if (y <= noise)
				{
					i32 index = (x - worldPosition.x) + 
						((y - worldPosition.y) * (CHUNK_SIZE + 2)) + 
						((z - worldPosition.z) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2));
					
					if (mountainNoise > 0.65 && smoothedNoise > 0.65)
					{
						f64 mountainBlockNoise = perlin.octave2D_01((f64)x * 0.1, (f64)z * 0.1, 8, 0.75);
						
						if (mountainBlockNoise <= 0.8)
						{
							m_Datas[index] = mountainNoise < 0.75 ? BlockType::Stone : BlockType::Snow;
						}
						else m_Datas[index] = (y == noise) ? BlockType::SnowGrassBlock : BlockType::Dirt;
					}
					else m_Datas[index] = (y == noise) ? BlockType::GrassBlock : BlockType::Dirt;

					changes++;
				}
			}
		}
	}

	if (changes == 0)
	{
		m_IsGenerationFinished = true;
		return;
	}

	for (u8 z = 0; z <= CHUNK_SIZE + 1; z++)
	{
		for (u8 x = 0; x <= CHUNK_SIZE + 1; x++)
		{
			for (u8 y = 0; y <= CHUNK_SIZE + 1; y++)
			{
				i32 index = x + (y * (CHUNK_SIZE + 2)) + (z * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2));

				BlockType currentBlockType = m_Datas[index];
				if (currentBlockType == BlockType::Air) continue;

				//glm::vec3 position{ worldPosition.x + x - 1, worldPosition.y + y - 1, worldPosition.z + z - 1 };

				//u32 position = EncodeCoords(static_cast<u8>(x), static_cast<u8>(y), static_cast<u8>(z));

				if (
					x == 0
					|| x > CHUNK_SIZE
					|| y == 0
					|| y > CHUNK_SIZE
					|| z == 0
					|| z > CHUNK_SIZE
					) continue;

				u8vec3 position{ x, y, z };

				i32 topIndex = index + (CHUNK_SIZE + 2);
				i32 bottomIndex = index - (CHUNK_SIZE + 2);
				i32 frontIndex = index + (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
				i32 backIndex = index - (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
				i32 rightIndex = index + 1;
				i32 leftIndex = index - 1;

				// check top neighbor
				if (m_Datas[topIndex] == BlockType::Air)
					AddFace(m_Vertices, m_Indices, CubeFace::Top, position, m_BlockManagerInstance->GetTextureHandleIndex(currentBlockType));
				
				if (currentBlockType == BlockType::GrassBlock) currentBlockType = BlockType::GrassBlockSide;
				if (currentBlockType == BlockType::SnowGrassBlock) currentBlockType = BlockType::SnowGrassBlockSide;

				// check front neighbor
				if (m_Datas[frontIndex] == BlockType::Air)
					AddFace(m_Vertices, m_Indices, CubeFace::Front, position, m_BlockManagerInstance->GetTextureHandleIndex(currentBlockType));
				// check back neighbor
				if (m_Datas[backIndex] == BlockType::Air)
					AddFace(m_Vertices, m_Indices, CubeFace::Back, position, m_BlockManagerInstance->GetTextureHandleIndex(currentBlockType));
				// check right nieghbor
				if (m_Datas[rightIndex] == BlockType::Air)
					AddFace(m_Vertices, m_Indices, CubeFace::Right, position, m_BlockManagerInstance->GetTextureHandleIndex(currentBlockType));
				// check left neighbor
				if (m_Datas[leftIndex] == BlockType::Air)
					AddFace(m_Vertices, m_Indices, CubeFace::Left, position, m_BlockManagerInstance->GetTextureHandleIndex(currentBlockType));

				if (currentBlockType == BlockType::GrassBlockSide
					|| currentBlockType == BlockType::SnowGrassBlockSide) currentBlockType = BlockType::Dirt;

				// check bottom neighbor
				if (m_Datas[bottomIndex] == BlockType::Air)
					AddFace(m_Vertices, m_Indices, CubeFace::Bottom, position, m_BlockManagerInstance->GetTextureHandleIndex(currentBlockType));
			}
		}
	}

	m_IsGenerationFinished = true;
}


void NewChunk::Render(Shader& sh)
{
	if (!m_VAO)
	{
		m_VBO = new Buffer(sizeof(Vertex) * m_Vertices.size(), m_Vertices.data());
		m_EBO = new Buffer(sizeof(u32) * m_Indices.size(), m_Indices.data());

		m_VAO = new VertexArray(m_VBO->GetId(), sizeof(Vertex), m_EBO->GetId());
		//m_VAO->LinkAttrib(0, 3, GL_FLOAT, offsetof(Vertex, Position));
		m_VAO->LinkAttribI(0, 1, GL_UNSIGNED_INT, offsetof(Vertex, Position));
		m_VAO->LinkAttrib(1, 3, GL_FLOAT, offsetof(Vertex, Normal));
		m_VAO->LinkAttrib(2, 2, GL_FLOAT, offsetof(Vertex, UV));
		m_VAO->LinkAttribI(3, 1, GL_UNSIGNED_INT, offsetof(Vertex, TextureIndex));
	}

	m_VAO->Bind();

	sh.SetVec3("chunkWorldPos", m_Settings.Position * (f32)CHUNK_SIZE);

	glDrawElements(GL_TRIANGLES, (i32)m_Indices.size(), GL_UNSIGNED_INT, 0);
}



