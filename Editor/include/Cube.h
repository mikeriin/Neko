#ifndef CUBE_H
#define CUBE_H


#include <vector>


#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using mat4 = glm::mat4;


#include "Neko.h"



struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
};


static void AddFace(std::vector<Vertex>& vertices, std::vector<u32>& indices, u8 faceIndex, const vec3& position = vec3{ 0.0 })
{
	u32 vertexAmount = (u32)vertices.size();

	switch (faceIndex)
	{
	case 1: // front face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, { -0.0f,  1.0f, }},
				{{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, { -0.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f,  1.0f, }}
			}
		);
		break;
	case 2: // right face
		vertices.insert(vertices.end(),
			{
				{{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  1.0f,  0.0f,  0.0f }, { -0.0f,  1.0f, }},
				{{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  1.0f,  0.0f,  0.0f }, { -0.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  1.0f,  0.0f,  0.0f }, {  1.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f, }}
			}
		);
		break;
	case 3: // back face
		vertices.insert(vertices.end(),
			{
				{{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, { -0.0f,  1.0f, }},
				{{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, { -0.0f, -0.0f, }},
				{{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, {  1.0f, -0.0f, }},
				{{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  0.0f, -1.0f }, {  1.0f,  1.0f, }}
			}
		);
		break;
	case 4: // left face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, }, { -1.0f,  0.0f,  0.0f }, { -0.0f,  1.0f, }},
				{{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, }, { -1.0f,  0.0f,  0.0f }, { -0.0f, -0.0f, }},
				{{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, }, { -1.0f,  0.0f,  0.0f }, {  1.0f, -0.0f, }},
				{{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, }, { -1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f, }}
			}
		);
		break;
	case 5: // top face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  1.0f,  0.0f }, { -0.0f,  1.0f, }},
				{{ position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  1.0f,  0.0f }, { -0.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, }, {  0.0f,  1.0f,  0.0f }, {  1.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, }, {  0.0f,  1.0f,  0.0f }, {  1.0f,  1.0f, }}
			}
		);
		break;
	case 6: // bottom face
		vertices.insert(vertices.end(),
			{
				{{ position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f, -1.0f,  0.0f }, { -0.0f,  1.0f, }},
				{{ position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f, -1.0f,  0.0f }, { -0.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, }, {  0.0f, -1.0f,  0.0f }, {  1.0f, -0.0f, }},
				{{ position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, }, {  0.0f, -1.0f,  0.0f }, {  1.0f,  1.0f, }}
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
