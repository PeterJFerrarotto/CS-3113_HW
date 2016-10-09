#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "matrix.h"
#include "ShaderProgram.h"
#include <math.h>
#include "Entity.h"
#include "CollisionEvent.h"
#include "TexturedEntity.h"
#include "Vector3.h"
#include <unordered_map>
#include "EventListener.h"
#include "TickEvent.h"
#include "CollisionEventListener.h"
#include "LogicEvent.h"
#include <time.h>
using namespace std;

unsigned int points;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define GAME_CEILING 2.0
#define GAME_WALL 3.55
#define PLAYER_SPEED 2.5
SDL_Window* displayWindow;

unordered_map<int, vector<Entity*>> gameEntities;
vector<float> projectileVertices = { -0.0625, -0.125, 0.0625, -.125, 0.0625, .125, -0.0625, -.125, 0.0625, .125, -0.0625, .125 };
vector<float> shipVertices = { -0.25, -0.125, 0.25, -0.125, 0.25, 0.125, -0.25, -0.125, 0.25, 0.125, -0.25, 0.125 };
float enemyFireDelay = 0;
GLuint projectileTextureID;

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

void handleInput(const Uint8* state, Entity* playerModel){

}

void draw(ShaderProgram* program){
	for (size_t i = 0; i < gameEntities.size(); i++){
		for (vector<Entity*>::const_iterator itr = gameEntities[i].begin(); itr != gameEntities[i].end(); itr++){
			if ((*itr)->getState() != DESTROYED)
				(*itr)->draw(program);
		}
	}
}

void createPlayerProjectile(GLuint bulletTexID, float playerCoordX = 0, float playerCoordY = -2){
	Entity* playerProjectile = new TexturedEntity();
	
	playerProjectile->setCanCollide(true);
	playerProjectile->setBoundaryBehavior(BOUND_DESTROY);
	playerProjectile->setCollisionType(SQUARE);
	playerProjectile->setEntityType(PLAYER_PROJECTILE);
	playerProjectile->setState(PROJECTILE_MOTION);
	playerProjectile->setCollisionBehavior(COLL_DESTROY);
	playerProjectile->setDefaultTextureVertices();
	playerProjectile->setObjectVertices(projectileVertices);
	playerProjectile->setBounding(0.0625, 0.125);
	playerProjectile->setPosition(playerCoordX, playerCoordY + 0.0625);
	playerProjectile->setVelocity(0, 1.2);
	playerProjectile->setTextureID(bulletTexID);
	gameEntities[PLAYER_PROJECTILE].push_back(playerProjectile);
}

void createEnemyShipProjectile(GLuint bulletTexID, Entity* parent){
	Entity* enemyProjectile = new TexturedEntity();

	enemyProjectile->setCanCollide(true);
	enemyProjectile->setBoundaryBehavior(BOUND_DESTROY);
	enemyProjectile->setCollisionType(SQUARE);
	enemyProjectile->setEntityType(ENEMY_PROJECTILE);
	enemyProjectile->setState(PROJECTILE_MOTION);
	enemyProjectile->setCollisionBehavior(COLL_DESTROY);
	enemyProjectile->setDefaultTextureVertices();
	enemyProjectile->setObjectVertices(projectileVertices);
	enemyProjectile->setBounding(0.0625, 0.125);
	enemyProjectile->setPosition(parent->getPosition().x, parent->getPosition().y);
	enemyProjectile->setVelocity(0, -1.2);
	enemyProjectile->setRotation(M_PI);
	enemyProjectile->setTextureID(bulletTexID);
	gameEntities[ENEMY_PROJECTILE].push_back(enemyProjectile);
}

void moveProjectiles(float elapsed, int projectileType){
	vector<Entity*> projectiles = gameEntities[projectileType];
	for (vector<Entity*>::iterator itr = projectiles.begin(); itr != projectiles.end(); itr++){
		Entity* projectile = (*itr);
		if (projectile->getState() == PROJECTILE_MOTION){
			if (abs(projectile->getPosition().y) - projectile->getBounding().y >= GAME_CEILING){
				projectile->setState(DESTROYED);
			}
		}
		float velX = projectile->getVelocity().x;
		float velY = projectile->getVelocity().y;
		float posX = projectile->getPosition().x;
		float posY = projectile->getPosition().y;

		posX += velX * elapsed;

		posY += velY * elapsed;

		projectile->setPosition(posX, posY);
	}
}

void cleanUpEntities(int entityType){
	if (gameEntities[entityType].size() > 0){
		vector<vector<Entity*>::iterator> itrsToErase;
		for (vector<Entity*>::iterator itr = gameEntities[entityType].begin(); itr != gameEntities[entityType].end(); itr++){
			if ((*itr)->getState() == DESTROYED){
				itrsToErase.push_back(itr);
				if (gameEntities[entityType].size() < 1){
					break;
				}
			}
		}
		if (itrsToErase.size() > 0){
			for (size_t i = 0; i < itrsToErase.size(); i++){
				gameEntities[entityType].erase(itrsToErase[i]);
			}
		}
		itrsToErase.clear();
	}
}

void fillShipData(GLuint textureID, Entity* enemy, float posX, float posY, int shipType){
	enemy->setCanCollide(true);
	enemy->setBoundaryBehavior(BOUND_TURN);
	enemy->setCollisionType(SQUARE);
	enemy->setEntityType(ENEMY_PROJECTILE);
	enemy->setState(ENEMY_ALIVE);
	enemy->setCollisionBehavior(COLL_DESTROY);
	enemy->setRoll(0);
	enemy->setYaw(0);
	enemy->setDefaultTextureVertices();
	enemy->setObjectVertices(shipVertices);
	enemy->setBounding(0.25, 0.06);
	enemy->setVelocity(0, 0);
	enemy->setTextureID(textureID);
	enemy->setPosition(posX, posY);
	gameEntities[shipType].push_back(enemy);
}

void projectileCollision(Entity* projectile, Entity* object){
	if ((projectile->getEntityType() != ENEMY_PROJECTILE || object->getEntityType() != ENEMY_SHIP || object->getEntityType() != ENEMY_SAUCER))
		if ((projectile->getEntityType() != PLAYER_PROJECTILE || object->getEntityType() != PLAYER) && object->getEntityType() != TEXT){
			if (projectile->getEntityType() == PLAYER_PROJECTILE && object->getEntityType() == ENEMY_SHIP){
				if (projectile->getPosition().y + projectile->getBounding().y > object->getPosition().y){
					return;
				}
				points += 150;
			}
			else if (projectile->getEntityType() == ENEMY_PROJECTILE && object->getEntityType() == PLAYER){
				if (projectile->getPosition().y - projectile->getBounding().y < object->getPosition().y){
					return;
				}
			}
			projectile->setState(DESTROYED);
			if (object->getCollisionBehavior() == COLL_DESTROY){
				object->setState(DESTROYED);
			}
		}
}

void movePlayer(float elapsed, int type){
	vector<Entity*> entities = gameEntities[type];
	vector<Entity*> p2 = gameEntities[PLAYER];
	for (Entity* player : gameEntities.at(type)){
		float playerPosX = player->getPosition().x;
		playerPosX += player->getVelocity().x * elapsed;
		if (abs(playerPosX) + player->getBounding().x >= GAME_WALL){
			playerPosX = player->getPosition().x;
		}
		player->setPosition(playerPosX, player->getPosition().y);
	}
}

void shipMotionLogic(float elapsed, int type){
	vector<Entity*> entities = gameEntities[type];
	bool turn = false;
	bool canCreateProjectile = true;
	int createdProjectiles = 0;
	for (Entity* ship : entities){
		float posX = ship->getPosition().x;
		float boundingX = ship->getBounding().x;
		float posY = ship->getPosition().y;
		float boundingY = ship->getBounding().y;
		float velX = ship->getVelocity().x;

		if (velX > 0){
			velX = (float)(1.5 / entities.size());
		}
		else{
			velX = -(float)(1.5 / entities.size());
		}
		posX += velX * elapsed;
		if (abs(posX) + boundingX >= GAME_WALL){
			turn = true;
		}

		ship->setPosition(posX, posY);
		ship->setVelocity(velX, 0);
		Entity* player = gameEntities[PLAYER][0];
		if (!(posX + boundingX < player->getPosition().x - player->getBounding().x || posX - boundingX > player->getPosition().x + player->getBounding().x)){
			canCreateProjectile = ship->getCanFire() && createdProjectiles == 0;
			if (gameEntities[ENEMY_PROJECTILE].size() > 0){
				canCreateProjectile = canCreateProjectile && gameEntities[ENEMY_PROJECTILE][gameEntities[ENEMY_PROJECTILE].size() - 1]->getPosition().y <= 0 && gameEntities[ENEMY_PROJECTILE].size() <= 10;
			}
			if (canCreateProjectile){
				createEnemyShipProjectile(projectileTextureID, ship);
				srand(time(NULL));
				ship->resetFiringDelay((float)(rand()));
				createdProjectiles++;
			}
		}
		ship->decreaseFiringDelay(elapsed);
	}
	if (turn){
		for (Entity* ship : entities){
			float posX = ship->getPosition().x;
			float posY = ship->getPosition().y;
			float velX = ship->getVelocity().x;

			posX -= velX * elapsed;
			velX = -velX;
			posX += velX * elapsed;

			posY -= 0.125;

			ship->setPosition(posX, posY);
			ship->setVelocity(velX, 0);
		}
	}
}

void generateRow(GLuint textureID, float rowYPosition){
	Entity* enemy1 = new TexturedEntity;
	Entity* enemy2 = new TexturedEntity;
	Entity* enemy3 = new TexturedEntity;
	Entity* enemy4 = new TexturedEntity;
	Entity* enemy5 = new TexturedEntity;
	Entity* enemy6 = new TexturedEntity;
	Entity* enemy7 = new TexturedEntity;
	
	fillShipData(textureID, enemy1, -2.25, rowYPosition, ENEMY_SHIP);
	fillShipData(textureID, enemy2, -1.5, rowYPosition, ENEMY_SHIP);
	fillShipData(textureID, enemy3, -0.75, rowYPosition, ENEMY_SHIP);
	fillShipData(textureID, enemy4, 0, rowYPosition, ENEMY_SHIP);
	fillShipData(textureID, enemy5, 0.75, rowYPosition, ENEMY_SHIP);
	fillShipData(textureID, enemy6, 1.5, rowYPosition, ENEMY_SHIP);
	fillShipData(textureID, enemy7, 2.25, rowYPosition, ENEMY_SHIP);
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
	GLuint playerTextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/playerShip1_blue.png");
	GLuint playerTextureID2 = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/playerShip2_blue.png");
	GLuint playerTextureID3 = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/playerShip3_blue.png");
	projectileTextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/Lasers/laserBlue01.png");
	GLuint enemyBlack1TextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/Enemies/enemyBlack1.png");
	GLuint enemyBlack2TextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/Enemies/enemyBlack2.png");
	GLuint enemyBlack3TextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/Enemies/enemyBlack3.png");
	GLuint enemyBlack4TextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/Enemies/enemyBlack4.png");
	GLuint enemyBlack5TextureID = loadTexture(RESOURCE_FOLDER"Assets/Space shooter assets (300 assets)/PNG/Enemies/enemyBlack5.png");

	GLuint textTextureID = loadTexture(RESOURCE_FOLDER"Assets/pixel_font.png");

	Entity* playerModel = new TexturedEntity;

	generateRow(enemyBlack1TextureID, 1.5);
	generateRow(enemyBlack2TextureID, 1.25);
	generateRow(enemyBlack3TextureID, 1.0);

	fillShipData(playerTextureID, playerModel, 0, -1.5, PLAYER);

	glViewport(0, 0, 640, 720);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	map<string, Entity*> enemyAIContainer;

	EventListener<TickEvent> tickListener;
	CollisionEventListener collisionListener;

	CollisionEvent playerProjectileWithEnemyShip;
	playerProjectileWithEnemyShip.setCollisionGroup1(&gameEntities[PLAYER_PROJECTILE]);
	playerProjectileWithEnemyShip.setCollisionGroup2(&gameEntities[ENEMY_SHIP]);
	playerProjectileWithEnemyShip.setCollisionEvent(&projectileCollision);

	collisionListener.addEvent(playerProjectileWithEnemyShip);

	CollisionEvent enemyProjectileWithPlayerShip;

	enemyProjectileWithPlayerShip.setCollisionGroup1(&gameEntities[ENEMY_PROJECTILE]);
	enemyProjectileWithPlayerShip.setCollisionGroup2(&gameEntities[PLAYER]);
	enemyProjectileWithPlayerShip.setCollisionEvent(&projectileCollision);

	collisionListener.addEvent(enemyProjectileWithPlayerShip);

	TickEvent playerProjectileMovement;
	playerProjectileMovement.setTickingEntities(PLAYER_PROJECTILE);
	playerProjectileMovement.setEventToDo(&moveProjectiles);

	tickListener.addEvent(playerProjectileMovement);

	TickEvent enemyProjectileMovement;
	enemyProjectileMovement.setTickingEntities(ENEMY_PROJECTILE);
	enemyProjectileMovement.setEventToDo(&moveProjectiles);

	tickListener.addEvent(enemyProjectileMovement);

	TickEvent playerMovement;
	playerMovement.setTickingEntities(PLAYER);
	playerMovement.setEventToDo(&movePlayer);

	tickListener.addEvent(playerMovement);

	TickEvent shipLogic;
	shipLogic.setTickingEntities(ENEMY_SHIP);
	shipLogic.setEventToDo(&shipMotionLogic);

	tickListener.addEvent(shipLogic);

	Matrix projectionMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	unsigned lives = 3;

	//Game Loop:
	SDL_Event event;
	bool done = false;
	bool pressed = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		gameEntities[PLAYER][0]->decreaseFiringDelay(elapsed);
		glClear(GL_COLOR_BUFFER_BIT);
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_SPACE]){
			if (gameEntities[PLAYER][0]->getCanFire()){
				createPlayerProjectile(projectileTextureID, gameEntities[PLAYER][0]->getPosition().x, gameEntities[PLAYER][0]->getPosition().y);
				gameEntities[PLAYER][0]->resetFiringDelay();
			}
		}


		if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]){
			gameEntities[PLAYER][0]->setVelocity(-PLAYER_SPEED, 0);
			gameEntities[PLAYER][0]->setRoll(-0.015);
		}
		else if (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT]){
			gameEntities[PLAYER][0]->setVelocity(PLAYER_SPEED, 0);
			gameEntities[PLAYER][0]->setRoll(0.015);
		}
		else{
			gameEntities[PLAYER][0]->setVelocity(0, 0);
			gameEntities[PLAYER][0]->setRoll(0);
		}
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		tickListener.listen(elapsed);
		collisionListener.listen();
		cleanUpEntities(ENEMY_SHIP);
		cleanUpEntities(PLAYER_PROJECTILE);
		cleanUpEntities(ENEMY_PROJECTILE);
		cleanUpEntities(PLAYER);
		draw(&program);

		SDL_GL_SwapWindow(displayWindow);

		if (gameEntities[PLAYER].size() == 0 && lives != 0){
			lives--;
			if (lives == 0){
				done = true;
			}
			Entity* newPlayerModel = new TexturedEntity();
			switch (lives){
			case(3) :
				break;
			case(2) :
				fillShipData(playerTextureID2, newPlayerModel, 0, -1.5, PLAYER);
				break;
			case(1) :
				fillShipData(playerTextureID3, newPlayerModel, 0, -1.5, PLAYER);
				break;
			default:
				break;
			}
		}

		if (gameEntities[ENEMY_SHIP].size() == 0){
			done = true;
		}

	}

	SDL_Quit();
	return 0;
}
