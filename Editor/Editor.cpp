

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

//#include "world/Chunk.h"
#include "world/BlockManager.h"
#include "world/NewChunk.h"
#include "Block.h"


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
	
	blockManager->MapTexture(Block::Debug, textureManager.AddTexture(new Texture("assets/textures/debug.png")));
	blockManager->MapTexture(Block::Dirt, textureManager.AddTexture(new Texture("assets/textures/dirt.png")));
	blockManager->MapTexture(Block::GrassBlock, textureManager.AddTexture(new Texture("assets/textures/grass_block_top.png")));
	blockManager->MapTexture(Block::GrassBlockSide, textureManager.AddTexture(new Texture("assets/textures/grass_block_side.png")));
	blockManager->MapTexture(Block::Stone, textureManager.AddTexture(new Texture("assets/textures/stone.png")));
	blockManager->MapTexture(Block::GoldBlock, textureManager.AddTexture(new Texture("assets/textures/gold_block.png")));
	blockManager->MapTexture(Block::Snow, textureManager.AddTexture(new Texture("assets/textures/snow.png")));

	textureManager.UpdateSSBO();



	ChunkSettings chunkSettings;
	chunkSettings.Size = 16;
	chunkSettings.Seed = 99999999u;

	std::thread generationThread;
	std::queue<glm::ivec3> chunkToGenerate;
	for (i32 i = -12; i < 12; i++)
	{
		for (i32 j = -12; j < 12; j++)
		{
			for (i32 k = -1; k < (i32)(chunkSettings.MaxHeight / chunkSettings.Size); k++)
			{
				chunkToGenerate.push(glm::ivec3(i, k, j));
			}
		}
	}
	std::vector<NewChunk*> chunks;
	NewChunk* currentChunk{ nullptr };
	std::unordered_map<NewChunk*, bool> chunkVisibility;
	bool isChunkLoading = false;



	
	i32 viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	f32 aspect = (f32)viewport[2] / viewport[3];

	CameraSettings camSettings{};
	camSettings.Position = vec3{ 0.0, chunkSettings.MaxHeight, 0.0 };
	camSettings.AspectRatio = aspect;
	camSettings.Speed = 50.0f;
	camSettings.SensitivityX = 0.45f;
	camSettings.sensitivityY = 0.35f;
	camSettings.Yaw = -90.0f;
	camSettings.Pitch = -80.0f;
	camSettings.FarPlane = 1000.0f;

	Camera mainCamera{ camSettings };

	//Shader mainShader{ "assets/shaders/main_vert.glsl", "assets/shaders/main_frag.glsl" };
	Shader mainShader{ "assets/shaders/main_vert.glsl", "assets/shaders/main_bindless_textures_frag.glsl" };
	mainShader.Link();
	mainShader.Use();

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


		textureManager.Bind(0);


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

			// Si le chunk est vide, on le supprime
			if (ch->IsChunkEmpty()) {
				delete ch;
				it = chunks.erase(it); // Efface le chunk et récupère l'itérateur suivant
				continue;
			}

			bool isInFrustum = true;
			static int frameCounter = 0;
			if (frameCounter++ % 10 == 0) {  // Teste la frustum tous les 5 frames
				// Récupère les paramètres du chunk
				ChunkSettings currentChunkSettings = ch->GetSettings();
				glm::vec4 worldPosition{
						(f64)(currentChunkSettings.Position.x) * currentChunkSettings.Size,
						(f64)(currentChunkSettings.Position.y) * currentChunkSettings.Size,
						(f64)(currentChunkSettings.Position.z) * currentChunkSettings.Size,
						1.0
				};

				// Multiplie par la matrice de vue et de projection
				glm::mat4 viewMatrix = mainCamera.GetViewMatrix();
				glm::mat4 projectionMatrix = mainCamera.GetProjectionMatrix();
				glm::vec4 worldPosClipSpace = projectionMatrix * viewMatrix * worldPosition;

				// Vérifie si le chunk est dans la frustum
				isInFrustum = (
					-worldPosClipSpace.w <= worldPosClipSpace.x && worldPosClipSpace.x <= worldPosClipSpace.w &&
					-worldPosClipSpace.w <= worldPosClipSpace.y && worldPosClipSpace.y <= worldPosClipSpace.w &&
					-worldPosClipSpace.w <= worldPosClipSpace.z && worldPosClipSpace.z <= worldPosClipSpace.w
					);

				chunkVisibility[ch] = isInFrustum;
			}
			else {
				// Utiliser le dernier résultat de test
				isInFrustum = chunkVisibility[ch];
			}

		  if (isInFrustum) ch->Render();
			++it;  // Passe au chunk suivant
		}


		mainWindow.RenderSwap();
	}

	if (generationThread.joinable()) generationThread.join();

	for (NewChunk* chunk : chunks) delete chunk;
	delete currentChunk;

	delete blockManager;

	return 0;
}