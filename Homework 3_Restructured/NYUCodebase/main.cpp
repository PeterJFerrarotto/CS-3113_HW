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
#include "Entity.h"
#include "Texture.h"
#include "CompositeEntity.h"
#include "Ship.h"
#include "GameEngine.h"
#include "CollisionListener.h"
#include "EnemyShip.h"
using namespace rapidxml;
using namespace std;

unsigned int points;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


SDL_Window* displayWindow;
vector<float> projectileVertices = { -0.0625, -0.125, 0.0625, -.125, 0.0625, .125, -0.0625, -.125, 0.0625, .125, -0.0625, .125 };
vector<float> shipVertices = { -0.373333335f, -0.250000000f, 0.373333335f, 0.250000000f, -0.373333335f, 0.250000000f, 0.373333335f, 0.250000000f, -0.373333335f, -0.250000000f, 0.373333335f, -0.250000000f };

GLuint loadTexture(const char* imagePath){
	SDL_Surface *surface = IMG_Load(imagePath);
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	GLenum externalFormat = GL_RGBA;
	if (surface->format->BytesPerPixel == 3) {
		externalFormat = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

xml_document<> * loadXMLFile(const char* filePath){
	file<>* xmlFile = new file<>(filePath);
	xml_document<>* doc = new xml_document<>;
	doc->parse<0>(xmlFile->data());
	return doc;
}

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
	
	GLuint spriteSheet = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/Spritesheet/sheet.png");
	file<> * xmlFile = new file<>(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/Spritesheet/sheet.xml");
	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(xmlFile->data());

	glViewport(0, 0, 640, 720);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture* projectileTexture = new Texture(spriteSheet, doc, "laserBlue01.png", 0.4, 0);

	GameEngine gameEngine(projectileTexture);

	Matrix projectionMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	Texture* tex = new Texture(spriteSheet, doc, "playerShip2_orange.png", 0.5, 0);

	Texture* tex2 = new Texture(spriteSheet, doc, "playerLife1_green.png", 0.5, 99);

	Texture* enemyTex1 = new Texture(spriteSheet, doc, "enemyBlack4.png", 0.5, 1);

	Entity* x1 = new Entity("x1", tex);
	Entity* x2 = new Entity("x2", tex2);


	x1->setPosition(0, 0, 0);
	x1->setVelocity(0, 0, 0);
	x1->setAcceleration(0, 0, 0);
	x1->setScale(1.5, 1, 1);
	x1->setBoundingType(SQUARE);
	x1->setRotation(0);
	x1->setCanCollide(true);

	x2->setPosition(0, 0, 0);
	x2->setVelocity(0, 0, 0);
	x2->setAcceleration(0, 0, 0);
	x2->setScale(1, 0.5, 1);
	x2->setBoundingType(SQUARE);
	x2->setRotation(0);
	x2->setCanCollide(false);


	CompositeEntity* X1 = new Ship(x1, 2.5);
	CompositeEntity* X2 = new Ship(x2, 2.5);



	X1->setPosition(0, -GAME_CEILING + 0.5);
	X1->setScale(0.5, 0.5);
	X1->setRotation(0);
	X1->setVelocity(0, 0, 0);
	X1->setBoundingType(SQUARE);
	X1->setEntityType(ACTOR_PLAYER);
	X1->setCollisionBehavior(DEACTIVATE);
	X1->setBoundaryBehavior(BOUND_STOP);
	X1->setIsActive(true);
	X1->setCanCollide(true);

	X2->setPosition(-GAME_WALL + 0.75, -GAME_CEILING + 0.25);
	X2->setScale(1, 1);
	X2->setRotation(0);
	X2->setBoundingType(SQUARE);
	X2->setEntities(x2);
	X2->setEntityType(ICON_ENTITY);
	X2->setCollisionBehavior(BOUNCE);
	X2->setBoundaryBehavior(BOUND_NOTHING);
	X2->setIsActive(true);
	X2->setCanCollide(false);

	for (float y = GAME_CEILING - 0.5; y > GAME_CEILING - 1.5; y -= 0.5){
		for (float x = -GAME_WALL + 0.5; x < -GAME_WALL + 6.0; x += 1.0){
			Entity* enemy = new Entity("enemy1", enemyTex1);

			enemy->setPosition(0, 0, 0);
			enemy->setVelocity(0, 0, 0);
			enemy->setAcceleration(0, 0, 0);
			enemy->setScale(1.5, 1, 1);
			enemy->setBoundingType(SQUARE);
			enemy->setRotation(0);
			enemy->setCanCollide(true);

			CompositeEntity* Enemy = new EnemyShip(enemy, 2.5);

			Enemy->setPosition(x, y);
			Enemy->setVelocity(0.8, 0);
			Enemy->setScale(0.5, 0.5);
			Enemy->setRotation(0);
			Enemy->setBoundingType(SQUARE);
			Enemy->setEntityType(ACTOR_ENEMY);
			Enemy->setCollisionBehavior(DEACTIVATE);
			Enemy->setBoundaryBehavior(BOUND_TURN_AND_DOWN);
			Enemy->setIsActive(true);
			Enemy->setCanCollide(true);
			gameEngine.addGameEntity(Enemy);
		}
	}


	gameEngine.addGameEntity(X2);
	gameEngine.addGameEntity(X1);


	CollisionListener* testCollision = new CollisionListener(ACTOR_PLAYER, ICON_ENTITY);
	CollisionListener* enemyWithProjectile = new CollisionListener(ACTOR_ENEMY, PLAYER_PROJECTILE);
	CollisionListener* playerWithProjectile = new CollisionListener(ACTOR_PLAYER, ENEMY_PROJECTILE);

	gameEngine.addCollisionEvent(testCollision);
	gameEngine.addCollisionEvent(enemyWithProjectile);
	gameEngine.addCollisionEvent(playerWithProjectile);

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
		

		gameEngine.updateEntities(elapsed, state, &program);

		done = gameEngine.getGameOver();

		SDL_GL_SwapWindow(displayWindow);
	}
	
	SDL_Quit();
	return 0;
}
