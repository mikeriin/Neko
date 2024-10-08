
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>


#include "Neko.h"
#include "core/Window.h"
#include "core/Event.h"
#include "core/Time.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Camera.h"


#include "glad/glad_wgl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using mat4 = glm::mat4;


#include "Cube.h"




int main(
	int argc, 
	char* argv[]
) {
	Window mainWindow{};
	EventHandler events{};

	Time time{};
	time.SetMaxFPS(0); // use Time::FixedDeltaTime
	
	if (!mainWindow.Create(1280, 720, "Widnow")) return -1;

	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "OpenGL Version: " << version << std::endl;


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.675f, 0.529f, 0.773f, 1.0f);




	Texture::SetPixelated(true);


	Texture dirtTex("assets/textures/dirt.png");
	//Texture grassBlockSideOverlayTex("assets/textures/grass_block_side_overlay.png");
	//Texture grassBlockTopTex("assets/textures/grass_block_top.png");

	u32 textureBuffer;
	glCreateBuffers(1, &textureBuffer);
	glNamedBufferStorage(textureBuffer, sizeof(u64) * Texture::TextureHandles.size(), Texture::TextureHandles.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, textureBuffer);

	



	std::vector<Vertex> vertices{};
	std::vector<u32> indices{};
	int blocks[10 * 10 * 10] = {};

	for (i32 z = 0; z < 10; z++)
	{
		for (i32 y = 0; y < 10; y++)
		{
			for (i32 x = 0; x < 10; x++)
			{
				i32 index = y * (10 * 10) + z * (10) + x;
				i32 randVal = rand() % 10;

				if (randVal > 3 && randVal < 7) blocks[index] = 1;
				else blocks[index] = 0;
			}
		}
	}


	for (i32 z = -5; z < 5; z++)
	{
		for (i32 y = -5; y < 5; y++)
		{
			for (i32 x = -5; x < 5; x++)
			{
				if (z == -5 || z == 4 || y == -5 || y == 4 || x == -5 || x == 4) continue;

				i32 index = (y + 5) * (10 * 10) + (z + 5) * (10) + (x + 5);

				if (blocks[index] == 0) continue;

				i32 z0 = (y + 5) * (10 * 10) + (z + 4) * (10) + (x + 5); // back
				if (blocks[z0] == 0) AddFace(vertices, indices, 3, vec3{ x, y, z }); // back
				i32 z1 = (y + 5) * (10 * 10) + (z + 6) * (10) + (x + 5); // front
				if (blocks[z1] == 0) AddFace(vertices, indices, 1, vec3{ x, y, z }); // front

				i32 x0 = (y + 5) * (10 * 10) + (z + 5) * (10) + (x + 4); // left
				if (blocks[x0] == 0) AddFace(vertices, indices, 4, vec3{ x, y, z }); // left
				i32 x1 = (y + 5) * (10 * 10) + (z + 5) * (10) + (x + 6); // right
				if (blocks[x1] == 0) AddFace(vertices, indices, 2, vec3{ x, y, z }); // right

				i32 y0 = (y + 4) * (10 * 10) + (z + 5) * (10) + (x + 5); // bottom
				if (blocks[y0] == 0) AddFace(vertices, indices, 6, vec3{ x, y, z }); // bottom
				i32 y1 = (y + 6) * (10 * 10) + (z + 5) * (10) + (x + 5); // top
				if (blocks[y1] == 0) AddFace(vertices, indices, 5, vec3{ x, y, z }); // top
			}
		}
	}



	u32 vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	u32 ibo;
	glCreateBuffers(1, &ibo);
	glNamedBufferStorage(ibo, sizeof(u32) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);

	u32 vao;
	glCreateVertexArrays(1, &vao);

	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex)); // binding 0

	glVertexArrayElementBuffer(vao, ibo);

	glEnableVertexArrayAttrib(vao, 0); // index 0
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position)); // index 0
	glVertexArrayAttribBinding(vao, 0, 0); // index 0 binding 0

	glEnableVertexArrayAttrib(vao, 1); // index 1
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal)); // index 1
	glVertexArrayAttribBinding(vao, 1, 0); // index 1 binding 0

	glEnableVertexArrayAttrib(vao, 2); // index 2
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, UV)); // index 2
	glVertexArrayAttribBinding(vao, 2, 0); // index 2 binding 0


	Shader mainShader;
	mainShader.AddShader("assets/shaders/main_vert.glsl", GL_VERTEX_SHADER);
	mainShader.AddShader("assets/shaders/bindless_textures_frag.glsl", GL_FRAGMENT_SHADER);
	//mainShader.AddShader("assets/shaders/main_frag.glsl", GL_FRAGMENT_SHADER);
	mainShader.Link();
	mainShader.Use();

	//mainShader.SetVec3("colorMult", vec3{ 0.596, 0.878, 0.439 });


	
	i32 viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	f32 aspect = (f32)viewport[2] / viewport[3];

	CameraSettings camSettings{};
	camSettings.Position = vec3{ 0.0, 5.0, 10.0 };
	camSettings.AspectRatio = aspect;
	camSettings.Speed = 8.0f;
	camSettings.SensitivityX = 0.3f;
	camSettings.sensitivityY = 0.2f;
	camSettings.Yaw = -90.0f;

	Camera mainCamera{ camSettings };

	mat4 transform{ 1.0 };
	mainShader.SetMat4("transform", transform);


	bool isWireframe = false;
	while (mainWindow.IsOpen())
	{
		time.Update();

		events.PollEvents();

		if (events.KeyJustPressed(KEY_TAB))
		{
			isWireframe = !isWireframe;
			glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
		}


		// update aspect ratio
		i32 newViewport[4];
		glGetIntegerv(GL_VIEWPORT, newViewport);

		if (newViewport[2] != viewport[2] || newViewport[3] != viewport[3])
		{
			aspect = (f32)newViewport[2] / newViewport[3];
			mainCamera.SetAspectRatio(aspect);
			memcpy(viewport, newViewport, 4 * sizeof(i32));
		}
		
		// sent cam datas to shaders
		mainCamera.Update(events, Time::DeltaTime);
		mainShader.SetMat4("view", mainCamera.GetViewMatrix());
		mainShader.SetMat4("projection", mainCamera.GetProjectionMatrix());


		// draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/*dirtTex.BindUnit(0);
		mainShader.SetInt("texBlock", 0);
		grassBlockSideOverlayTex.BindUnit(1);
		mainShader.SetInt("texOverlay", 1);
		grassBlockTopTex.BindUnit(2);
		mainShader.SetInt("texTop", 2);*/

		//for (u64 handle : Texture::TextureHandles) glMakeTextureHandleResidentARB(handle);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);

		//for (u64 handle : Texture::TextureHandles) glMakeTextureHandleNonResidentARB(handle);



		mainWindow.RenderSwap();
	}

	return 0;
}