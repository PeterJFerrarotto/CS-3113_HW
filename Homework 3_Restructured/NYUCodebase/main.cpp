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
#include "Ship.h"
#include "CollisionListener.h"
#include "EnemyShip.h"
#include "TextEntity.h"
#include "GameEngine.h"
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
	glViewport(0, 0, 640, 720);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Matrix projectionMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	GLuint spriteSheet = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/Spritesheet/sheet.png");
	GLuint font = loadTexture(RESOURCE_FOLDER"Assets/font2.png");
	file<> * xmlFile = new file<>(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/Spritesheet/sheet.xml");
	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(xmlFile->data());	

	Texture* projectileTexture = new Texture(spriteSheet, doc, "laserBlue01.png", 0.4, 0);

	GameEngine gameEngine(projectileTexture);


	Texture* tex = new Texture(spriteSheet, doc, "playerShip2_orange.png", 0.5, 0);

	Texture* iconTexture = new Texture(spriteSheet, doc, "playerLife1_green.png", 0.5, 99);

	Texture* staticTexture = new Texture(spriteSheet, doc, "meteorBrown_big1.png", 0.5, 99);

	Texture* enemyTexture = new Texture(spriteSheet, doc, "enemyBlack4.png", 0.5, 1);

	Entity* playerEntity = new Entity("playerEntity", tex);
	Entity* iconEntity = new Entity("iconEntity", iconTexture);
	Entity* staticTest = new Entity("staticTest", staticTexture);

	playerEntity->setPosition(0, 0, 0);
	playerEntity->setVelocity(0, 0, 0);
	playerEntity->setAcceleration(0, 0, 0);
	playerEntity->setScale(1.5, 1, 1);
	playerEntity->setBoundingType(SQUARE);
	playerEntity->setRotation(0);
	playerEntity->setCanCollide(true);
	playerEntity->setDoRender(true);

	iconEntity->setPosition(0, 0, 0);
	iconEntity->setVelocity(0, 0, 0);
	iconEntity->setAcceleration(0, 0, 0);
	iconEntity->setScale(1, 0.5, 1);
	iconEntity->setBoundingType(SQUARE);
	iconEntity->setRotation(0);
	iconEntity->setCanCollide(false);
	iconEntity->setDoRender(true);

	staticTest->setPosition(0, 0, 0);
	staticTest->setVelocity(0, 0, 0);
	staticTest->setAcceleration(0, 0, 0);
	staticTest->setScale(1.5, 1, 1);
	staticTest->setBoundingType(SQUARE);
	staticTest->setRotation(0);
	staticTest->setCanCollide(true);
	staticTest->setDoRender(true);

	CompositeEntity* playerShip = new Ship(playerEntity, 1.0);
	CompositeEntity* lifeIcon = new CompositeEntity(iconEntity);
	CompositeEntity* lifeIcon2 = new CompositeEntity(iconEntity);
	CompositeEntity* lifeIcon3 = new CompositeEntity(iconEntity);
	CompositeEntity* titleText = new TextEntity("Space Invaders", font, 0.2, 0.05, -GAME_WALL + 1.0, GAME_CEILING - 0.8);
	CompositeEntity* textSelection1 = new TextEntity("Begin Game!", font, 0.2, 0.05, -GAME_WALL + 1.0, 0);
	CompositeEntity* pointsIndicator = new TextEntity("Points: ", font, 0.1, 0.05, -GAME_WALL + 0.8, GAME_CEILING - 0.1);

	playerShip->setStartingPosition(0, -GAME_CEILING + 0.5);
	playerShip->setStartingScale(0.5, 0.5);
	playerShip->setRotation(0);
	playerShip->setStartingVelocity(0, 0, 0);
	playerShip->setBoundingType(SQUARE);
	playerShip->setEntityType( ACTOR_PLAYER);
	playerShip->setCollisionBehavior(DEACTIVATE);
	playerShip->setBoundaryBehavior(BOUND_STOP);
	playerShip->setIsActive(false);
	playerShip->setEntityID("Player");
	playerShip->setCanCollide(true);

	lifeIcon->setStartingPosition(-GAME_WALL + 0.75, -GAME_CEILING + 0.25);
	lifeIcon->setStartingScale(1, 1);
	lifeIcon->setRotation(0);
	lifeIcon->setBoundingType(SQUARE);
	lifeIcon->setEntities(iconEntity);
	lifeIcon->setEntityType( LIFE_ICON_ENTITY);
	lifeIcon->setCollisionBehavior(BOUNCE);
	lifeIcon->setBoundaryBehavior(BOUND_NOTHING);
	lifeIcon->setIsActive(false);
	lifeIcon->setEntityID("lifeIndicator");
	lifeIcon->setCanCollide(false);

	lifeIcon2->setStartingPosition(-GAME_WALL + 1.5, -GAME_CEILING + 0.25);
	lifeIcon2->setStartingScale(1, 1);
	lifeIcon2->setRotation(0);
	lifeIcon2->setBoundingType(SQUARE);
	lifeIcon2->setEntities(iconEntity);
	lifeIcon2->setEntityType( LIFE_ICON_ENTITY);
	lifeIcon2->setCollisionBehavior(BOUNCE);
	lifeIcon2->setBoundaryBehavior(BOUND_NOTHING);
	lifeIcon2->setIsActive(false);
	lifeIcon2->setEntityID("lifeIndicator");
	lifeIcon2->setCanCollide(false);

	lifeIcon3->setStartingPosition(-GAME_WALL + 2.25, -GAME_CEILING + 0.25);
	lifeIcon3->setStartingScale(1, 1);
	lifeIcon3->setRotation(0);
	lifeIcon3->setBoundingType(SQUARE);
	lifeIcon3->setEntities(iconEntity);
	lifeIcon3->setEntityType( LIFE_ICON_ENTITY);
	lifeIcon3->setCollisionBehavior(BOUNCE);
	lifeIcon3->setBoundaryBehavior(BOUND_NOTHING);
	lifeIcon3->setIsActive(false);
	lifeIcon3->setEntityID("lifeIndicator");
	lifeIcon3->setCanCollide(false);

	titleText->setStartingScale(1, 1);
	titleText->setBoundingType(SQUARE);
	titleText->setCanCollide(false);
	titleText->setIsActive(false);
	titleText->setEntityID("Title");
	titleText->setEntityType( TITLE_TEXT_ENTITY);

	textSelection1->setStartingScale(1, 1);
	textSelection1->setBoundingType(SQUARE);
	textSelection1->setCanCollide(false);
	textSelection1->setIsActive(false);
	textSelection1->setEntityID("Selection01");
	textSelection1->setEntityType( TITLE_TEXT_ENTITY);

	pointsIndicator->setStartingScale(1, 1);
	pointsIndicator->setCanCollide(false);
	pointsIndicator->setIsActive(false);
	pointsIndicator->setEntityID("Points");
	pointsIndicator->setEntityType( POINTS_INDICATOR);

	CompositeEntity* staticTestComp = new CompositeEntity(staticTest);
	staticTestComp->setCanCollide(true);
	staticTestComp->setIsActive(false);
	staticTestComp->setStartingPosition(1, -GAME_CEILING + 0.5);
	staticTestComp->setStartingScale(1, 1);
	staticTestComp->setRotation(0);
	staticTestComp->setStartingVelocity(0, 0, 0);
	staticTestComp->setBoundingType(SQUARE);
	staticTestComp->setEntityType(STATIC_ENTITY);
	staticTestComp->setCollisionBehavior(NOTHING);
	staticTestComp->setBoundaryBehavior(BOUND_NOTHING);

	for (float y = GAME_CEILING - 0.5; y > GAME_CEILING - 1.5; y -= 0.5){
		for (float x = -GAME_WALL + 0.5; x < -GAME_WALL + 6.0; x += 1.0){
			Entity* enemy = new Entity("enemy", enemyTexture);

			enemy->setPosition(0, 0, 0);
			enemy->setVelocity(0, 0, 0);
			enemy->setAcceleration(0, 0, 0);
			enemy->setScale(1.5, 1, 1);
			enemy->setBoundingType(SQUARE);
			enemy->setRotation(0);
			enemy->setCanCollide(true);
			enemy->setDoRender(true);

			CompositeEntity* Enemy = new EnemyShip(enemy, 2.5);

			Enemy->setStartingPosition(x, y);
			Enemy->setStartingVelocity(0.8, 0);
			Enemy->setStartingScale(0.5, 0.5);
			Enemy->setRotation(0);
			Enemy->setBoundingType(SQUARE);
			Enemy->setEntityType( ACTOR_ENEMY);
			Enemy->setCollisionBehavior(DEACTIVATE);
			Enemy->setBoundaryBehavior(BOUND_NOTHING);
			Enemy->setIsActive(false);
			Enemy->setCanCollide(true);
			Enemy->setEntityID("enemy");
			Enemy->updateBounding();
			gameEngine.addGameEntity(Enemy);
		}
	}


	gameEngine.addGameEntity(lifeIcon);
	gameEngine.addGameEntity(lifeIcon2);
	gameEngine.addGameEntity(lifeIcon3);
	gameEngine.addGameEntity(playerShip);
	gameEngine.addGameEntity(titleText);
	gameEngine.addGameEntity(textSelection1);
	gameEngine.addGameEntity(pointsIndicator);
	gameEngine.addGameEntity(staticTestComp);

	CollisionListener* enemyWithProjectile = new CollisionListener( ACTOR_ENEMY,  PLAYER_PROJECTILE);
	CollisionListener* playerWithProjectile = new CollisionListener( ACTOR_PLAYER,  ENEMY_PROJECTILE);

	gameEngine.addCollisionEvent(enemyWithProjectile);
	gameEngine.addCollisionEvent(playerWithProjectile);

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
