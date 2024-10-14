#ifndef CUBE_H
#define CUBE_H


#include <vector>
#include <map>


#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using mat4 = glm::mat4;


#include "Neko.h"



struct Vertex
{
	vec3	Position;
	vec3	Normal;
	vec2	UV;
	u32		TextureIndex;
};


enum class CubeFace
{
	Front,
	Right,
	Back,
	Left,
	Top,
	Bottom,
};


enum Block;


static void AddFace(
	std::vector<Vertex>& vertices, 
	std::vector<u32>& indices, 
	CubeFace faceIndex, 
	const vec3& position = vec3{ 0.0 }, 
	u32 textureIndex = 0)
{
	u32 vertexAmount = (u32)vertices.size();

	switch (faceIndex)
	{
	case CubeFace::Front: // front face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, { -0.0f,  1.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, { -0.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f,  1.0f, }, textureIndex}
			}
		);
		break;
	case CubeFace::Right: // right face
		vertices.insert(vertices.end(),
			{
				{{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  1.0f,  0.0f,  0.0f }, { -0.0f,  1.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  1.0f,  0.0f,  0.0f }, { -0.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  1.0f,  0.0f,  0.0f }, {  1.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f, }, textureIndex}
			}
		);
		break;
	case CubeFace::Back: // back face
		vertices.insert(vertices.end(),
			{
				{{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, { -0.0f,  1.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, { -0.0f, -0.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, {  1.0f, -0.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, {  1.0f,  1.0f, }, textureIndex}
			}
		);
		break;
	case CubeFace::Left: // left face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, }, { -1.0f,  0.0f,  0.0f }, { -0.0f,  1.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, }, { -1.0f,  0.0f,  0.0f }, { -0.0f, -0.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, }, { -1.0f,  0.0f,  0.0f }, {  1.0f, -0.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, }, { -1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f, }, textureIndex}
			}
		);
		break;
	case CubeFace::Top: // top face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  1.0f,  0.0f }, { -0.0f,  1.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  1.0f,  0.0f }, { -0.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  1.0f,  0.0f }, {  1.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  1.0f,  0.0f }, {  1.0f,  1.0f, }, textureIndex}
			}
		);
		break;
	case CubeFace::Bottom: // bottom face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f, -1.0f,  0.0f }, { -0.0f,  1.0f, }, textureIndex},
				{{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f, -1.0f,  0.0f }, { -0.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f, -1.0f,  0.0f }, {  1.0f, -0.0f, }, textureIndex},
				{{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f, -1.0f,  0.0f }, {  1.0f,  1.0f, }, textureIndex}
			}
		);
		break;

	default:
		break;
	}



	indices.insert(indices.end(), {
		vertexAmount,
		vertexAmount + 1,
		vertexAmount + 3,

		vertexAmount + 3,
		vertexAmount + 1,
		vertexAmount + 2
		});
}


#endif // !CUBE_H
