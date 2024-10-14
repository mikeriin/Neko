#include "world/NewChunk.h"


#include <cmath>
#include <algorithm>

#include <glad/glad.h>

#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"

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

	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	glm::ivec3 woldRelativePosition = glm::ivec3{
		m_Settings.Position.x * m_Settings.Size,
		m_Settings.Position.y * m_Settings.Size,
		m_Settings.Position.z * m_Settings.Size,
	};


	for (i32 z = (i32)(woldRelativePosition.z - 1); z <= (i32)(woldRelativePosition.z + m_Settings.Size); z++)
	{
		for (i32 x = (i32)(woldRelativePosition.x - 1); x <= (i32)(woldRelativePosition.x + m_Settings.Size); x++)
		{
			f64 biomeNoise = perlin.noise2D_01((f64)x * 0.001, (f64)z * 0.001);
			f64 mountainNoise = perlin.octave2D_01((f32)x * 0.015, (f32)z * 0.015, 8, 0.45);
			f64 plainNoise = perlin.octave2D_01((f32)x * 0.001, (f32)z * 0.001, 4);


			f64 transitionFactor = smoothstep(0.55, 0.7, biomeNoise);
			f64 interpolatedNoise = lerp(plainNoise * 32, mountainNoise * m_Settings.MaxHeight, transitionFactor);


			i32 noise = (i32)(interpolatedNoise);


			for (i32 y = (i32)(woldRelativePosition.y - 1); y < noise; y++)
			{
				if (mountainNoise < 0.625 && plainNoise > 0.625)
				{
					if (y == noise - 1) m_Datas.emplace(std::make_tuple(x, y, z), Block::GrassBlock);
					else m_Datas.emplace(std::make_tuple(x, y, z), Block::Dirt);
				}
				else
				{
					if (y > 64)
					{
						if (mountainNoise > 0.8) m_Datas.emplace(std::make_tuple(x, y, z), Block::Snow);
						else m_Datas.emplace(std::make_tuple(x, y, z), Block::Stone);
					}
					else if (y == noise - 1) m_Datas.emplace(std::make_tuple(x, y, z), Block::GrassBlock);
					else m_Datas.emplace(std::make_tuple(x, y, z), Block::Dirt);
				}
			}
		}
	}

	//if (m_Datas.empty()) return;

	for (const auto& [position, block] : m_Datas)
	{
		i32 x = std::get<0>(position);
		i32 y = std::get<1>(position);
		i32 z = std::get<2>(position);
		glm::vec3 blockPos{ x, y, z };


		bool isTopAir = m_Datas.find(std::make_tuple(x, y + 1, z)) == m_Datas.end();
		bool isBottomAir = m_Datas.find(std::make_tuple(x, y - 1, z)) == m_Datas.end();
		bool isFrontAir = m_Datas.find(std::make_tuple(x, y, z + 1)) == m_Datas.end();
		bool isBackAir = m_Datas.find(std::make_tuple(x, y, z - 1)) == m_Datas.end();
		bool isRightAir = m_Datas.find(std::make_tuple(x + 1, y, z)) == m_Datas.end();
		bool isLeftAir = m_Datas.find(std::make_tuple(x - 1, y, z)) == m_Datas.end();

		if (
			z < (i32)(woldRelativePosition.z)
			|| z >= (i32)(woldRelativePosition.z + m_Settings.Size)
			|| x < (i32)(woldRelativePosition.x)
			|| x >= (i32)(woldRelativePosition.x + m_Settings.Size)
			|| y < (i32)(woldRelativePosition.y)
			|| y >= (i32)(woldRelativePosition.y + m_Settings.Size)
			) continue;

		u32 textureIndex = m_BlockManagerInstance->GetTextureHandleIndex(block);
		glm::ivec3 facePosition{ x, y, z };


		if (isTopAir) AddFace(m_Vertices, m_Indices, CubeFace::Top, facePosition, textureIndex);

		if (block == Block::GrassBlock) textureIndex = m_BlockManagerInstance->GetTextureHandleIndex(Block::GrassBlockSide);
		if (isFrontAir) AddFace(m_Vertices, m_Indices, CubeFace::Front, facePosition, textureIndex);
		if (isBackAir) AddFace(m_Vertices, m_Indices, CubeFace::Back, facePosition, textureIndex);
		if (isRightAir) AddFace(m_Vertices, m_Indices, CubeFace::Right, facePosition, textureIndex);
		if (isLeftAir) AddFace(m_Vertices, m_Indices, CubeFace::Left, facePosition, textureIndex);

		if (block == Block::GrassBlock) textureIndex = m_BlockManagerInstance->GetTextureHandleIndex(Block::Dirt);
		if (isBottomAir) AddFace(m_Vertices, m_Indices, CubeFace::Bottom, facePosition, textureIndex);
	}

	m_IsGenerationFinished = true;
}


void NewChunk::Render()
{
	if (!m_VAO)
	{
		m_VBO = new Buffer(sizeof(Vertex) * m_Vertices.size(), m_Vertices.data());
		m_EBO = new Buffer(sizeof(u32) * m_Indices.size(), m_Indices.data());

		m_VAO = new VertexArray(m_VBO->GetId(), sizeof(Vertex), m_EBO->GetId());
		m_VAO->LinkAttrib(0, 3, GL_FLOAT, offsetof(Vertex, Position));
		m_VAO->LinkAttrib(1, 3, GL_FLOAT, offsetof(Vertex, Normal));
		m_VAO->LinkAttrib(2, 2, GL_FLOAT, offsetof(Vertex, UV));
		m_VAO->LinkAttribI(3, 1, GL_UNSIGNED_INT, offsetof(Vertex, TextureIndex));
	}

	m_VAO->Bind();
	glDrawElements(GL_TRIANGLES, (i32)m_Indices.size(), GL_UNSIGNED_INT, 0);
}



