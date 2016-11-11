#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "matrix.h"
#include "ShaderProgram.h"
#include <math.h>
#include <unordered_map>
#include <time.h>
#include <iostream>
#include <fstream>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "Texture.h"
#include "Entity.h"
#include "CompositeEntity.h"
#include "CollisionListener.h"
#include "EnemyShip.h"
#include "TextEntity.h"
#include "GameEngine.h"
#include "dirent.h"
#include "XMLHelper.h"
using namespace rapidxml;
using namespace std;

unsigned int points;



SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	float lastFrameTicks = 0.0f;
	glViewport(0, 0, 640, 720);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Matrix projectionMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	GameEngine gameEngine;

	CollisionListener* enemyProjectileWithPlayer = new CollisionListener(ACTOR_PLAYER, ENEMY_PROJECTILE);
	CollisionListener* playerProjectileWithEnemy = new CollisionListener(ACTOR_ENEMY, PLAYER_PROJECTILE);

	gameEngine.addCollisionEvent(enemyProjectileWithPlayer);
	gameEngine.addCollisionEvent(playerProjectileWithEnemy);

	loadXMLData(gameEngine);
	gameEngine.start();
	//Game Loop:
	SDL_Event event;
	int gameState = TITLE_SCREEN;
	bool done = false;
	bool pressed = false;
	int titleSelection = 0;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		glClear(GL_COLOR_BUFFER_BIT);
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		gameEngine.updateEntities(elapsed, state, event, &program);
		
		done = gameEngine.getGameOver();

		SDL_GL_SwapWindow(displayWindow);
	}
	
	SDL_Quit();
	return 0;
}
