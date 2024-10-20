

#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <queue>


#include "Neko.h"

#include "core/Window.h"
#include "core/Event.h"
#include "core/Time.h"

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Camera.h"
#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"
#include "graphics/BindlessTextureManager.h"

#include "utils/RandomNumber.h"

//#include "world/Chunk.h"
#include "world/BlockManager.h"
#include "world/NewChunk.h"
#include "Block.h"

#include "utils/BlockType.h"


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using mat4 = glm::mat4;




int main(int argc, char* argv[]) 
{
	Window mainWindow{};
	EventHandler events{};

	Time time{};
	//time.SetMaxFPS(60); // use Time::FixedDeltaTime
	
	if (!mainWindow.Create(1600, 900, "Widnow")) return -1;

	


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.675f, 0.529f, 0.773f, 1.0f);



	Texture::SetPixelated(true);
	BindlessTextureManager textureManager;
	BlockManager* blockManager = BlockManager::GetInstance();
	
	blockManager->MapTexture(BlockType::Error, textureManager.AddTexture(new Texture("assets/textures/debug.png")));
	blockManager->MapTexture(BlockType::Dirt, textureManager.AddTexture(new Texture("assets/textures/dirt.png")));
	blockManager->MapTexture(BlockType::GrassBlock, textureManager.AddTexture(new Texture("assets/textures/grass_block_top.png")));
	blockManager->MapTexture(BlockType::GrassBlockSide, textureManager.AddTexture(new Texture("assets/textures/grass_block_side.png")));
	blockManager->MapTexture(BlockType::Stone, textureManager.AddTexture(new Texture("assets/textures/stone.png")));
	blockManager->MapTexture(BlockType::Gold, textureManager.AddTexture(new Texture("assets/textures/gold_block.png")));
	blockManager->MapTexture(BlockType::Snow, textureManager.AddTexture(new Texture("assets/textures/snow.png")));
	blockManager->MapTexture(BlockType::SnowGrassBlock, blockManager->GetTextureHandleIndex(BlockType::Snow));
	blockManager->MapTexture(BlockType::SnowGrassBlockSide, textureManager.AddTexture(new Texture("assets/textures/snow_grass_block_side.png")));

	textureManager.UpdateSSBO();



	ChunkSettings chunkSettings;
	chunkSettings.Seed = (u32)RngFromRange(100000, 999999);

	std::cout << chunkSettings.Seed << "\n";

	std::thread generationThread;
	std::queue<glm::ivec3> chunkToGenerate;

	const i32 VIEW_DISTANCE = 12;

	for (int r = 0; r <= VIEW_DISTANCE; r++) {
		for (int x = -r; x <= r; x++) {
			for (int z = -r; z <= r; z++) {
				if (!(abs(x) == r) && !(abs(z) == r)) continue;
				for (i32 y = 0; y < (MAX_GENERATION_HEIGHT / CHUNK_SIZE); y++)
				{
					chunkToGenerate.push(glm::ivec3(x, y, z));
				}
			}
		}
	}

	std::vector<NewChunk*> chunks;
	NewChunk* currentChunk{ nullptr };
	std::unordered_map<NewChunk*, bool> chunkVisibility;
	bool isChunkLoading = false;


	std::vector<u32> testPositions
	{
		{ static_cast<u8>(0) << 8 | static_cast<u8>(1) << 4 | static_cast<u8>(0) },
		{ static_cast<u8>(0) << 8 | static_cast<u8>(0) << 4 | static_cast<u8>(0) },
		{ static_cast<u8>(1) << 8 | static_cast<u8>(1) << 4 | static_cast<u8>(0) },

		{ static_cast<u8>(1) << 8 | static_cast<u8>(1) << 4 | static_cast<u8>(0) },
		{ static_cast<u8>(0) << 8 | static_cast<u8>(0) << 4 | static_cast<u8>(0) },
		{ static_cast<u8>(1) << 8 | static_cast<u8>(0) << 4 | static_cast<u8>(0) },
	};

	Buffer testVBO{ sizeof(u32) * testPositions.size(), testPositions.data() };

	VertexArray testVAO{ testVBO.GetId(), sizeof(u32)};
	testVAO.LinkAttribI(0, 1, GL_UNSIGNED_INT, 0);




	
	i32 viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	f32 aspect = (f32)viewport[2] / viewport[3];

	CameraSettings camSettings{};
	camSettings.Position = vec3{ 0.0, MAX_GENERATION_HEIGHT * 3, 0.0 };
	camSettings.AspectRatio = aspect;
	camSettings.Speed = 50.0f;
	camSettings.SensitivityX = 0.45f;
	camSettings.sensitivityY = 0.35f;
	camSettings.Yaw = -90.0f;
	//camSettings.Pitch = -80.0f;
	camSettings.FarPlane = 1000.0f;

	Camera mainCamera{ camSettings };

	//Shader mainShader{ "assets/shaders/main_vert.glsl", "assets/shaders/main_frag.glsl" };
	Shader mainShader{ "assets/shaders/main_vert.glsl", "assets/shaders/main_bindless_textures_frag.glsl" };
	mainShader.Link();
	mainShader.Use();

	mat4 transform{ 1.0 };
	mainShader.SetMat4("transform", transform);



	/*Shader testShader{ "assets/shaders/test/vert.glsl", "assets/shaders/test/frag.glsl" };
	testShader.Link();
	testShader.Use();*/

	//testShader.SetMat4("transform", transform);


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
		//mainShader.Use();
		mainShader.SetMat4("view", mainCamera.GetViewMatrix());
		mainShader.SetMat4("projection", mainCamera.GetProjectionMatrix());

		/*testShader.Use();
		testShader.SetMat4("view", mainCamera.GetViewMatrix());
		testShader.SetMat4("projection", mainCamera.GetProjectionMatrix());*/

		// draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		textureManager.Bind(0);
		//mainShader.Use();

		if (!chunkToGenerate.empty() && !isChunkLoading)
		{
			glm::ivec3 chPos = chunkToGenerate.front();

			chunkSettings.Position = chPos;
			currentChunk = new NewChunk(chunkSettings);

			generationThread = std::thread([&currentChunk]() {
				currentChunk->Generate();
				});
			generationThread.detach();

			isChunkLoading = true;
			chunkToGenerate.pop();
		}

		if (currentChunk && currentChunk->IsGenerated())
		{
			chunks.push_back(currentChunk);
			isChunkLoading = false;
			currentChunk = nullptr;
		}

		for (auto it = chunks.begin(); it != chunks.end(); ) {
			NewChunk* ch = *it;

			if (ch->IsChunkEmpty()) {
				delete ch;
				it = chunks.erase(it);
				continue;
			}

			ch->Render(mainShader);
			++it;
		}
		//mainShader.Unuse();


		/*testShader.Use();
		testVAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		testShader.Unuse();*/


		mainWindow.RenderSwap();
	}

	if (generationThread.joinable()) generationThread.join();

	for (NewChunk* chunk : chunks) delete chunk;
	delete currentChunk;

	delete blockManager;

	return 0;
}