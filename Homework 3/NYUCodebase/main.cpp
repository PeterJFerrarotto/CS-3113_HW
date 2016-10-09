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
#include "SpriteEntity.h"
#include "LogicEvent.h"
#include <time.h>
using namespace std;

unsigned int points;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GAME_STATE {TITLE_SCREEN, GAME_BEGIN, GAME_PLAY, GAME_END, GAME_QUIT};

#define GAME_CEILING 2.0
#define GAME_WALL 3.55
#define PLAYER_SPEED 2.5
SDL_Window* displayWindow;

unordered_map<int, vector<Entity*>> gameEntities;
vector<float> projectileVertices = { -0.0625, -0.125, 0.0625, -.125, 0.0625, .125, -0.0625, -.125, 0.0625, .125, -0.0625, .125 };
vector<float> shipVertices = { -0.25, -0.125, 0.25, -0.125, 0.25, 0.125, -0.25, -0.125, 0.25, 0.125, -0.25, 0.125 };
vector<float> textVertices = { -5, -5, 5, -5, 5, 5, -5, -5, 5, 5, -5, 5 };
int activeEnemies = 0;
float enemyFireDelay = 0;
GLuint projectileTextureID;
bool enemiesWin;

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
			if ((*itr)->getRender())
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
	playerProjectile->setRender(true);
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
	enemyProjectile->setRender(true);
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

void cleanUpProjectiles(int entityType){
	if (gameEntities[entityType].size() > 0){
		for (vector<Entity*>::iterator itr = gameEntities[entityType].begin(); itr != gameEntities[entityType].end(); itr++){
			if ((*itr)->getState() == DESTROYED){
				vector<Entity*>::iterator newItr = gameEntities[entityType].erase(itr);
				itr = newItr;
				if (newItr == gameEntities[entityType].end()){
					break;
				}
			}
		}
	}
}

void fillShipData(GLuint textureID, Entity* enemy, float posX, float posY, int shipType){
	enemy->setCanCollide(true);
	enemy->setBoundaryBehavior(BOUND_TURN);
	enemy->setCollisionType(SQUARE);
	enemy->setEntityType(ENEMY_SHIP);
	enemy->setState(DESTROYED);
	enemy->setCollisionBehavior(COLL_DESTROY);
	enemy->setRoll(0);
	enemy->setYaw(0);
	enemy->setDefaultTextureVertices();
	enemy->setObjectVertices(shipVertices);
	enemy->setBounding(0.25, 0.06);
	enemy->setVelocity(0, 0);
	enemy->setTextureID(textureID);
	enemy->setPosition(posX, posY);
	enemy->setRender(false);
	enemy->setStartingPosition(posX, posY);
	enemy->setStartingVelocity(0, 0);
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
				activeEnemies--;
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
		if (ship->getState() != DESTROYED){
			float posX = ship->getPosition().x;
			float boundingX = ship->getBounding().x;
			float posY = ship->getPosition().y;
			float boundingY = ship->getBounding().y;
			float velX = ship->getVelocity().x;

			if (velX > 0){
				if (activeEnemies == 32){
					velX = 0.5;
				}

				else if (activeEnemies < 32 && activeEnemies >= 25){
					velX = 0.6;
				}

				else if (activeEnemies < 25 && activeEnemies >= 10){
					velX = 0.8;
				}

				else if (activeEnemies < 10 && activeEnemies > 1){
					velX = 1;
				}

				else if (activeEnemies == 1){
					velX = 1.5;
				}
			}
			else{
				if (activeEnemies == 32){
					velX = -0.5;
				}

				else if (activeEnemies < 32 && activeEnemies >= 25){
					velX = -0.6;
				}

				else if (activeEnemies < 25 && activeEnemies >= 10){
					velX = -0.8;
				}

				else if (activeEnemies < 10 && activeEnemies > 1){
					velX = -1;
				}

				else if (activeEnemies == 1){
					velX = -1.5;
				}
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
					canCreateProjectile = canCreateProjectile && gameEntities[ENEMY_PROJECTILE][gameEntities[ENEMY_PROJECTILE].size() - 1]->getPosition().y <= ship->getPosition().y - 1 && gameEntities[ENEMY_PROJECTILE].size() <= 10;
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
	}
	if (turn){
		for (Entity* ship : entities){
			if (ship->getState() != DESTROYED){
				float posX = ship->getPosition().x;
				float posY = ship->getPosition().y;
				float velX = ship->getVelocity().x;

				posX -= velX * elapsed;
				velX = -velX;
				posX += velX * elapsed;

				posY -= 0.0625;

				ship->setPosition(posX, posY);
				ship->setVelocity(velX, 0);
				enemiesWin = ship->getPosition().y <= -3;
			}
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

void manageRendering(int type){
	for (Entity* toEval : gameEntities[type]){
		if (toEval->getState() == DESTROYED || toEval->getState() == TEXT_INVISIBLE){
			toEval->setRender(false);
			toEval->setCanCollide(false);
		}
		else{
			toEval->setRender(true);
			if (toEval->getEntityType() != TEXT){
				toEval->setCanCollide(true);
			}
		}
	}
}

void drawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing, float startingX, float startingY) {
	Matrix modelMatrix;
	modelMatrix.identity();
	modelMatrix.Translate(startingX, startingY, 0.0);
	float texture_size = 1.0 / 16.0f; std::vector<float> vertexData; std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y,
			texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
		});
	}
	program->setModelMatrix(modelMatrix);
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data()); 
	glEnableVertexAttribArray(program->positionAttribute); 
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute); 
	glBindTexture(GL_TEXTURE_2D, fontTexture); 
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
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

	GLuint textTextureID = loadTexture(RESOURCE_FOLDER"Assets/font2.png");

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
		gameEntities[PLAYER][0]->decreaseFiringDelay(elapsed);
		glClear(GL_COLOR_BUFFER_BIT);
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		manageRendering(PLAYER);
		manageRendering(ENEMY_SHIP);
		manageRendering(TEXT);

		//TITLE_SCREEN State:
		if (gameState == TITLE_SCREEN){
			drawText(&program, textTextureID, "SPACE INVADERS", 0.2, 0.1, -1.25, 1.0);
			Uint8 up = state[SDL_SCANCODE_UP];
			Uint8 down = state[SDL_SCANCODE_DOWN];
			Uint8 enter = state[SDL_SCANCODE_RETURN];
			if (up){
				if (titleSelection == 1){
					titleSelection = 0;
				}
			}

			if (down){
				if (titleSelection == 0){
					titleSelection = 1;
				}
			}

			if (titleSelection == 0){
				drawText(&program, textTextureID, ">> START GAME", 0.2, 0.1, -1.0, 0);
				drawText(&program, textTextureID, "QUIT GAME", 0.2, 0.1, -1.0, -1);
			}
			else{
				drawText(&program, textTextureID, "START GAME", 0.2, 0.1, -1.0, 0);
				drawText(&program, textTextureID, ">> QUIT GAME", 0.2, 0.1, -1.0, -1);
			}
			if (enter){
				if (titleSelection == 0){
					gameState = GAME_BEGIN;
				}
				else{
					gameState = GAME_QUIT;
				}
			}
		}

		//GAME_BEGIN State:
		else if (gameState == GAME_BEGIN){
			for (Entity* toDo : gameEntities[PLAYER]){
				toDo->setState(ALIVE);
				toDo->setRender(true);
				toDo->returnToStart();
			}

			for (Entity* toDo : gameEntities[ENEMY_SHIP]){
				toDo->setState(ALIVE);
				toDo->setRender(true);
				toDo->returnToStart();
			}
			enemiesWin = false;
			activeEnemies = 21;
			gameState = GAME_PLAY;
			lives = 3;
		}
		
		//GAME_PLAY State:
		else if (gameState == GAME_PLAY){
			string livesCount = "Lives: " + to_string(lives);
			string scoreCount = "Score: " + to_string(points);

			drawText(&program, textTextureID, livesCount, 0.1, 0.1, -3.4, -1.7);
			drawText(&program, textTextureID, scoreCount, 0.1, 0.1, -1.5, 1.7);

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

			tickListener.listen(elapsed);
			collisionListener.listen();
			cleanUpProjectiles(PLAYER_PROJECTILE);
			cleanUpProjectiles(ENEMY_PROJECTILE);


			if (gameEntities[PLAYER][0]->getState() == DESTROYED){
				lives--;
				if (lives == 0){
					enemiesWin = true;
					gameState = GAME_END;
				}
				else{
					gameEntities[PLAYER][0]->setState(ALIVE);
				}
				switch (lives){
				case(3) :
					break;
				case(2) :
					gameEntities[PLAYER][0]->setTextureID(playerTextureID2);
					break;
				case(1) :
					gameEntities[PLAYER][0]->setTextureID(playerTextureID3);
					break;
				default:
					break;
				}
			}

			if (activeEnemies == 0){
				gameState = GAME_END;
				enemiesWin = false;
			}

			if (enemiesWin){
				gameState = GAME_END;
			}
		}

		//GAME_END State:
		else if (gameState == GAME_END){
			titleSelection = 0;
			for (pair<int, std::vector<Entity*>> toErase : gameEntities){
				for (Entity* toModify : toErase.second){
					toModify->setRender(false);
					toModify->setState(DESTROYED);
				}
			}
			cleanUpProjectiles(ENEMY_PROJECTILE);
			cleanUpProjectiles(PLAYER_PROJECTILE);
			gameState = TITLE_SCREEN;
		}

		//GAME_QUIT State:
		else if (gameState == GAME_QUIT){
			done = true;
		}

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		draw(&program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
