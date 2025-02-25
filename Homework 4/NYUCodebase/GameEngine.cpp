#include "GameEngine.h"
#include "CompositeEntity.h"
#include "CollisionListener.h"
#include "Texture.h"
#include "Entity.h"
#include "Level.h"
#include "Tile.h"
#include "ShaderProgram.h"
using namespace std;

GameEngine::GameEngine() : gameOver(false), titleSelection(0)
{
	enemyCount = 0;
}

GameEngine::~GameEngine()
{
}

void GameEngine::render(ShaderProgram* program, float elapsed, float fps){
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(0.03921568627, 0.59607843137, 0.67450980392, 1.0);
	uIMatrix.identity();
	viewMatrix.identity();
	viewMatrix.setScale(0.02, 0.01, 1);
	if (playerEntity->getPosition().x >= 180 && playerEntity->getPosition().x <= mapWidth - 180){
		viewMatrix.Translate(-playerEntity->getPosition().x, 0, 0);
		uIMatrix.Translate(playerEntity->getPosition().x - 172, 0, 0);
	}
	else if (playerEntity->getPosition().x < 180){
		viewMatrix.Translate(-180, 0, 0);
		uIMatrix.Translate(8, 0, 0);
	}
	else if (playerEntity->getPosition().x > mapWidth - 180){
		viewMatrix.Translate(-(int)(mapWidth - 180), 0, 0);
		uIMatrix.Translate((int)(mapWidth - 160), 0, 0);
	}
	
	if (playerEntity->getPosition().y >= -(int)(mapHeight - 200) && playerEntity->getPosition().y <= -16){
		viewMatrix.Translate(0, -playerEntity->getPosition().y, 0);
		uIMatrix.Translate(0, playerEntity->getPosition().y + 180, 0);
	}
	else if (playerEntity->getPosition().y < -(int)(mapHeight - 200)){
		viewMatrix.Translate(0, mapHeight - 200, 0);
		uIMatrix.Translate(0, -(int)(mapHeight - 380), 0);
	}
	else if (playerEntity->getPosition().y > -16){
		viewMatrix.Translate(0, 16, 0);
		uIMatrix.Translate(0, -32, 0);
	}

	program->setViewMatrix(viewMatrix);
	Level* lvl = levels[currentLevel];
	for (size_t i = 0; i < gameEntitiesRenderingOrder[currentLevel].size(); i++){
		levels[currentLevel]->draw(program, i);
		for (vector<CompositeEntity*>::const_iterator itr = gameEntitiesRenderingOrder[currentLevel][i].begin(); itr != gameEntitiesRenderingOrder[currentLevel][i].end(); itr++){
			if ((*itr)->getIsActive() && (*itr)->getEntityType() != POINTS_INDICATOR) 
				(*itr)->draw(program, uIMatrix, elapsed, fps);
		}
	}
	for (size_t i = 0; i < userInterface.size(); i++){
		userInterface[i]->draw(program, uIMatrix, elapsed, fps);
	}
}

void GameEngine::addGameEntity(const std::string& level, CompositeEntity* entity){
	if (entity->getEntityType() != BACKGROUND_ENTITY){
		gameEntities[level][entity->getEntityType()].push_back(entity);
		gameEntitiesRenderingOrder[level][entity->getLayer()].push_back(entity);
		//if (entity->getEntityType() == ACTOR_PLAYER){
		//	playerEntity = entity;
		//}
		if (entity->getEntityType() == ACTOR_ENEMY){
			enemyCount++;
		}
		if (entity->getEntityType() == POINTS_INDICATOR){
			userInterface.push_back(entity);
		}
	}
}

void GameEngine::addCollisionEvent(CollisionListener* collisionEvent){
	collisionEvents.push_back(collisionEvent);
}

void GameEngine::collisionCheck(float elapsed){
	for (size_t i = 0; i < collisionEvents.size(); i++){
		(*collisionEvents[i])(elapsed, gameEntities[currentLevel]);		
	}
}

void GameEngine::setLevel(const std::string& levelID){
	//if (backGroundTextures.find(levelID) == backGroundTextures.end()){
	//	throw "Unknown levelID!";
	//}
	deActivateLevelEntities();
	this->currentLevel = levelID;
	activateLevelEntities();
	this->mapHeight = levels[levelID]->getMapHeight();
	this->mapWidth = levels[levelID]->getMapLength();
	this->playerEntity = levels[levelID]->getPlayerEntity();
}

void GameEngine::addLevel(Level* level){
	if (levels.find(level->getLevelID()) != levels.end()){
		throw "Level already exists!";
	}
	levels[level->getLevelID()] = level;
}

float GameEngine::getTilePenetrationDown(float posY, float sizeY, int tileY, float hitY, float hitHeight){
	return abs(posY - sizeY) - (levels[currentLevel]->getTileSize() * (tileY) + hitY) + 0.001;
}

float GameEngine::getTilePenetrationUp(float posY, float sizeY, int tileY, float hitY, float hitHeight){
	return ((posY + sizeY)) - (-(levels[currentLevel]->getTileSize() * (tileY) + (hitHeight - hitY))) + 0.001;
}

float GameEngine::getTilePenetrationLeft(float posX, float sizeX, int tileX, float hitX, float hitWidth){
	return ((levels[currentLevel]->getTileSize() * (tileX)) + (hitWidth - hitX)) - (posX - sizeX) + 0.001;
}

float GameEngine::getTilePenetrationRight(float posX, float sizeX, int tileX, float hitX, float hitWidth){
	return (posX + sizeX) - ((levels[currentLevel]->getTileSize() * tileX) + hitX) + 0.001;
}

//Tile collision up if top of object is higher than bottom of tile and bottom of tile is higher than bottom of object
bool GameEngine::tileCollisionUp(float posY, float sizeYPos, float sizeYNeg, int tileY, float hitY, float hitHeight){
	return ((posY + sizeYPos) >= (-((tileY) * levels[currentLevel]->getTileSize() + (hitHeight - hitY)))) && ((-((tileY) * levels[currentLevel]->getTileSize() + (hitHeight - hitY))) >= (posY - sizeYNeg));
}

//Tile collision down if top of tile is higher than bottom of object and top of object is higher than top of tile
bool GameEngine::tileCollisionDown(float posY, float sizeYNeg, float sizeYPos, int tileY, float hitY, float hitHeight){
	//return posY - sizeYNeg >= (-(tileY + 1) * levels[currentLevel]->getTileSize());
	return (-((tileY)* levels[currentLevel]->getTileSize() + hitY) >= posY - sizeYNeg) && (posY + sizeYPos >= -((tileY)*levels[currentLevel]->getTileSize() + hitY));
}

//Tile collision left if left of object is farther right than left of tile
bool GameEngine::tileCollisionLeft(float posX, float sizeXNeg, float sizeXPos, int tileX, float hitX, float hitWidth){
	return posX - sizeXNeg >= ((tileX) * levels[currentLevel]->getTileSize() + hitX);
}

//Tile collision right if right of tile is farther right than right of object
bool GameEngine::tileCollisionRight(float posX, float sizeXPos, float sizeXNeg, int tileX, float hitX, float hitWidth){
	//return ((tileX) * levels[currentLevel]->getTileSize()) >= posX + sizeX;
	return ((tileX) * levels[currentLevel]->getTileSize() + (hitWidth - hitX)) >= posX + sizeXPos;
}

float GameEngine::checkBottomTileCollisions(CompositeEntity* entity){
	float posX = entity->getPosition().x;
	float posY = entity->getPosition().y;
	float sizeXPos = entity->getSizePositive().x/2;
	float sizeXNeg = entity->getSizeNegative().x / 2;
	float sizeY = entity->getSizeNegative().y/2;
	float tileSize = levels[currentLevel]->getTileSize();
	int gridRight = (int)((posX + sizeXPos) / tileSize);
	int gridLeft = (int)((posX - sizeXNeg) / tileSize);
	int gridBottom = (int)(floor(abs((posY-sizeY)/tileSize)));
	unsigned layer = entity->getLayer();
	float penetration = 0;

	Level* tmp = levels[currentLevel];
	Tile* tmpTile;
	if (abs(posY - sizeY) < mapHeight){
		for (int i = gridLeft; i <= gridRight; i++){
			tmpTile = tmp->getTile(layer, i, gridBottom);
			if (tmpTile != nullptr){
				if ((tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == SOLID_PLATFORM || tmpTile->tileType == SOLID_GRASS) && tmpTile->canCollide){
					if (tileCollisionDown(posY, sizeY, entity->getSizePositive().y / 2, gridBottom, tmpTile->y, tmpTile->height) && !(tmpTile->tileType == SOLID_PLATFORM && downPressed)){
						if (!(posY - sizeY < -(gridBottom * tileSize + tileSize / 2))){
							if (getTilePenetrationDown(posY, sizeY, gridBottom, tmpTile->y, tmpTile->height) > penetration && entity->getVelocity().y <= 0){
								penetration = getTilePenetrationDown(posY, sizeY, gridBottom, tmpTile->y, tmpTile->height);
								if (entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN){
									if (gridLeft * tileSize - (posX - sizeXNeg) <= 0.0001){
										tmpTile = tmp->getTile(layer, gridLeft, gridBottom);
										if (tmpTile == nullptr || tmpTile->canCollide == false){
											tmpTile = tmp->getTile(layer, gridLeft, gridBottom - 1);
											if (tmpTile == nullptr || tmpTile->canCollide == false){
												entity->setVelocity(-entity->getVelocity().x, entity->getVelocity().y);
											}
										}
									}
									if (gridRight * tileSize - (posX + sizeXPos) <= 0.0001){
										tmpTile = tmp->getTile(layer, gridRight, gridBottom);
										if (tmpTile == nullptr || tmpTile->canCollide == false){
											tmpTile = tmp->getTile(layer, gridRight, gridBottom - 1);
											if (tmpTile == nullptr || tmpTile->canCollide == false){
												entity->setVelocity(-entity->getVelocity().x, entity->getVelocity().y);
											}
										}

									}
								}
							}
						}
					}
				}
				else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
					entity->setIsActive(false);
					return 0;
				}
				else if (tmpTile->tileType == CHECKPOINT){
					entity->checkPoint();
					return 0;
				}
			}
		}
	}
	return penetration;
}

float GameEngine::checkTopTileCollisions(CompositeEntity* entity){
	float penetration = 0;
	float posX = entity->getPosition().x;
	float posY = entity->getPosition().y;
	float sizeXNeg = entity->getSizeNegative().x/2;
	float sizeXPos = entity->getSizePositive().x / 2;
	float sizeY = entity->getSizePositive().y/2;
	float tileSize = levels[currentLevel]->getTileSize();
	int mapHeightTiles = (int)(mapHeight / tileSize);
	int gridLeft = (int)((posX - sizeXNeg) / tileSize);
	int gridRight = (int)((posX + sizeXPos) / tileSize);
	int gridTop = abs((int)((posY + sizeY) / tileSize));
	unsigned layer = entity->getLayer();

	Level* tmp = levels[currentLevel];
	Tile* tmpTile;

	if (gridTop < mapHeightTiles && posY + sizeY < 0){
		for (int i = gridLeft; i <= gridRight; i++){
			tmpTile = tmp->getTile(layer, i, gridTop);
			if (tmpTile != nullptr){
				if ((tmpTile->tileType == SOLID || tmpTile->tileType == SOLID_GRASS || tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL) && tmpTile->canCollide){
					if (tileCollisionUp(posY, sizeY, entity->getSizeNegative().y / 2, gridTop, tmpTile->y, tmpTile->height)){
						//Check if object is below half-point of the tile
						if (getTilePenetrationUp(posY, sizeY, gridTop, tmpTile->y, tmpTile->height) > penetration && getTilePenetrationUp(posY, sizeY, gridTop, tmpTile->y, tmpTile->height) < tileSize){
							penetration = getTilePenetrationUp(posY, sizeY, gridTop, tmpTile->y, tmpTile->height);
						}
					}
				}


				else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
					entity->setIsActive(false);
					return 0;
				}
				else if (tmpTile->tileType == CHECKPOINT){
					entity->checkPoint();
					return 0;
				}
			}
		}
	}
	return penetration;
}

float GameEngine::checkLeftTileCollisions(CompositeEntity* entity){
	float penetration = 0;
	float posX = entity->getPosition().x;
	float posY = entity->getPosition().y;
	float sizeX = entity->getSizeNegative().x/2;
	float sizeYPos = entity->getSizePositive().y / 2;
	float sizeYNeg = entity->getSizeNegative().y / 2;
	float tileSize = levels[currentLevel]->getTileSize();
	int gridLeft = (int)((posX - sizeX) / tileSize);
	int gridBottom = abs((int)((posY - sizeYNeg) / tileSize));
	int gridTop = abs((int)((posY + sizeYPos) / tileSize));
	unsigned layer = entity->getLayer();

	Level* tmp = levels[currentLevel];
	Tile* tmpTile;
	for (int i = gridTop; i <= gridBottom; i++){
		tmpTile = tmp->getTile(layer, gridLeft, i);
		if (tmpTile != nullptr){
			if ((tmpTile->tileType == SOLID || tmpTile->tileType == SOLID_GRASS || tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL) && tmpTile->canCollide){
				if (tileCollisionLeft(posX, sizeX, entity->getSizePositive().x / 2, gridLeft, tmpTile->x, tmpTile->width)){
					if (getTilePenetrationLeft(posX, sizeX, gridLeft, tmpTile->x, tmpTile->width) > penetration){
						penetration = getTilePenetrationLeft(posX, sizeX, gridLeft, tmpTile->x, tmpTile->width);
					}
				}
			}

			else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
				entity->setIsActive(false);
				return 0;
			}
			else if (tmpTile->tileType == CHECKPOINT){
				entity->checkPoint();
				return 0;
			}
		}
	}
	return penetration;
}

float GameEngine::checkRightTileCollisions(CompositeEntity* entity){
	float posX = entity->getPosition().x;
	float posY = entity->getPosition().y;
	float sizeX = entity->getSizePositive().x/2;
	float sizeYPos = entity->getSizePositive().y/2;
	float sizeYNeg = entity->getSizeNegative().y / 2;
	float tileSize = levels[currentLevel]->getTileSize();
	int mapHeightTiles = (int)(mapHeight / tileSize);
	int gridRight = (int)((posX + sizeX) / tileSize);
	int gridBottom = abs((int)((posY - sizeYNeg) / tileSize));
	int gridTop = abs((int)((posY + sizeYPos) / tileSize));
	unsigned layer = entity->getLayer();
	float penetration = 0;

	Level* tmp = levels[currentLevel];
	Tile* tmpTile;
	for (int i = gridTop; i <= gridBottom; i++){
		tmpTile = tmp->getTile(layer, gridRight, i);
		if (tmpTile != nullptr){
			if ((tmpTile->tileType == SOLID || tmpTile->tileType == SOLID_GRASS || tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL) && tmpTile->canCollide){
				if (tileCollisionRight(posX, sizeX, entity->getSizeNegative().x / 2, gridRight, tmpTile->x, tmpTile->width)){
					if (getTilePenetrationRight(posX, sizeX, gridRight, tmpTile->x, tmpTile->width) > penetration){
						penetration = getTilePenetrationRight(posX, sizeX, gridRight, tmpTile->x, tmpTile->width);
					}
				}
			}
			else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
				entity->setIsActive(false);
				return 0;
			}
			else if (tmpTile->tileType == CHECKPOINT){
				entity->checkPoint();
				return 0;
			}
		}
	}
	return penetration;
}

void GameEngine::resolveYCollisions(CompositeEntity* entity){
	if (entity->getPosition().y - entity->getSizeNegative().y/2 <= -(int)(mapHeight)){
		entity->setIsActive(false);
	}
	else{
		float posY = entity->getPosition().y;
		float accY = entity->getAcceleration().y;
		float penetrationUp = checkTopTileCollisions(entity);
		float penetrationDown = checkBottomTileCollisions(entity);
		if (entity->getIsActive()){
			float velY = ((penetrationUp <= 0 && penetrationDown > 0) || (penetrationDown <= 0 && penetrationUp > 0)) ? 0 : entity->getVelocity().y;
			if (penetrationUp <= 0 && penetrationDown > 0){
				if (entity->getEntityType() == PLAYER_PROJECTILE){
					entity->destroy();
					return;
				}
				posY += penetrationDown;
				entity->setOnTileGround(true);
				accY = 0;
			}
			else if (penetrationDown <= 0 && penetrationUp > 0){
				if (entity->getEntityType() == PLAYER_PROJECTILE){
					entity->destroy();
					return;
				}
				posY -= penetrationUp;
				entity->setOnTileGround(false);
				accY = 0;
				entity->setState(FALLING);
			}
			else if (penetrationDown > 0 && penetrationUp > 0){
				if (entity->getEntityType() == PLAYER_PROJECTILE){
					entity->destroy();
					return;
				}
				posY += penetrationDown;
				entity->setOnTileGround(true);
				accY = 0;
			}
			else{
				entity->setOnTileGround(false);
			}
			entity->setPosition(entity->getPosition().x, posY);
			entity->setVelocity(entity->getVelocity().x, velY);
			entity->setAcceleration(entity->getAcceleration().x, accY);
		}
	}
}

void GameEngine::resolveXCollisions(CompositeEntity* entity){
	if (entity->getIsActive()){
		float posX = entity->getPosition().x;
		float velX = entity->getVelocity().x;
		float accX = entity->getAcceleration().x;
		float penetrationLeft = checkLeftTileCollisions(entity);
		float penetrationRight = checkRightTileCollisions(entity);
		if (penetrationRight <= 0 && penetrationLeft > 0){
			if (entity->getEntityType() == PLAYER_PROJECTILE){
				entity->destroy();
				return;
			}
			posX += penetrationLeft;
			velX = entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN ? -velX : 0;
			accX = 0;
			entity->setState(STATIONARY);
		}
		else if (penetrationLeft <= 0 && penetrationRight > 0){
			if (entity->getEntityType() == PLAYER_PROJECTILE){
				entity->destroy();
				return;
			}
			posX -= penetrationRight;
			velX = entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN ? -velX : 0;
			accX = 0;
		}
		entity->setPosition(posX, entity->getPosition().y);
		entity->setVelocity(velX, entity->getVelocity().y);
		entity->setAcceleration(accX, entity->getAcceleration().y);
	}
}

void GameEngine::resolveXYCollision(CompositeEntity* entity){
	if (entity->getPosition().y - entity->getSizeNegative().y / 2 <= -(int)(mapHeight)){
		entity->setIsActive(false);
	}
	else{
		float posX = entity->getPosition().x;
		float velX = entity->getVelocity().x;
		float accX = entity->getAcceleration().x;
		float posY = entity->getPosition().y;
		float velY = entity->getVelocity().y;
		float accY = entity->getAcceleration().y;
		
	}
}

void GameEngine::checkTileCollisions(){
	//Using layer, get value of the tile that the evaluated entity is currently touching
	//for (size_t i = 0; i < gameEntities[currentLevel].size(); i++){
	if (currentLevel == "level_02"){
		int x = 5;
	}
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			if (entity->getIsActive() && entity->getCanCollide() && entity->getState() != DESTROYING){
				resolveYCollisions(entity);
				resolveXCollisions(entity);
			}
		}
	}
}

void GameEngine::handleInput(const Uint8* input, SDL_Event input2){
	if (input[SDL_SCANCODE_ESCAPE]){
		gameState = GAME_QUIT;
	}

	if (input[SDL_SCANCODE_1]){
		if (gameState == GAME_PLAY){
			setLevel("level_01");
		}
	}

	if (input[SDL_SCANCODE_2]){
		if (gameState == GAME_PLAY){
			setLevel("level_02");
		}
	}

	if (input[SDL_SCANCODE_3]){
		if (gameState == GAME_PLAY){
			setLevel("level_03");
		}
	}

	if (input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getIsActive()){
			playerEntity->setDoMirror(true);
			playerEntity->setOverrideMirroring(true);
			if (playerEntity->getVelocity().x <= -playerEntity->getTopSpeed()){
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setVelocity(-playerEntity->getTopSpeed(), playerEntity->getVelocity().y);
			}
			else if (playerEntity->getVelocity().x > 0 && playerEntity->getVelocity().x < 10){
				playerEntity->setVelocity(0, playerEntity->getVelocity().y);
				playerEntity->setAcceleration(-PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
			}
			else{
				playerEntity->setAcceleration(-PLAYER_ACCELERATION * 5, playerEntity->getAcceleration().y);
			}
		}
	}

	if (input[SDL_SCANCODE_RIGHT] && !input[SDL_SCANCODE_LEFT]){
		if (playerEntity->getIsActive()){
			playerEntity->setDoMirror(false);
			playerEntity->setOverrideMirroring(true);
			if (playerEntity->getVelocity().x >= playerEntity->getTopSpeed()){
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setVelocity(playerEntity->getTopSpeed(), playerEntity->getVelocity().y);
			}
			else if (playerEntity->getVelocity().x < 0 && playerEntity->getVelocity().x > -5){
				playerEntity->setVelocity(0, playerEntity->getVelocity().y);
				playerEntity->setAcceleration(PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
			}
			else{
				playerEntity->setAcceleration(PLAYER_ACCELERATION * 5, playerEntity->getAcceleration().y);
			}
		}
	}

	if (!input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getIsActive()){
			playerEntity->setOverrideMirroring(false);
			if (!playerEntity->atScreenBoundary(0, mapWidth, -(int)(mapHeight), 0)){
				if (abs(playerEntity->getVelocity().x) >= 5){
					if (playerEntity->getVelocity().x > 0){
						playerEntity->setAcceleration(-PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
					}
					else if (playerEntity->getVelocity().x < 0){
						playerEntity->setAcceleration(PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
					}
					else{
						playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
					}
				}
				else{
					playerEntity->setVelocity(0, playerEntity->getVelocity().y);
					playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
					playerEntity->setState(STATIONARY);
				}
			}
			else{
				playerEntity->setVelocity(0, playerEntity->getVelocity().y);
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setState(STATIONARY);
			}
		}
	}

	downPressed = input[SDL_SCANCODE_DOWN];

	if (input[SDL_SCANCODE_UP]){
		if (gameState == GAME_PLAY && playerEntity->getIsActive()){
			if (playerEntity->getIsActive() && playerEntity->getOnGround()){
				playerEntity->jump();
			}
		}
	}

	if (input[SDL_SCANCODE_SPACE]){
		if (gameState == GAME_PLAY && playerEntity->getIsActive()){
			CompositeEntity* projectile = playerEntity->fire();
			if (projectile != nullptr){
				addGameEntity(currentLevel, projectile);
			}
		}
	}
	
	if (input[SDL_SCANCODE_R]){
		if (gameState == GAME_PLAY){
			changeState(GAME_END);
		}
	}

	if (input2.type == SDL_KEYDOWN){
		if (gameState == TITLE_SCREEN){
			if (input2.key.keysym.scancode == SDL_SCANCODE_UP || input2.key.keysym.scancode == SDL_SCANCODE_DOWN){
				if (titleSelection == 0){
					titleSelection = 1;
				}
				else{
					titleSelection = 0;
				}
				changeTitleSelection();
			}

			if (input2.key.keysym.scancode == SDL_SCANCODE_RETURN){
				if (titleSelection == 0){
					changeState(GAME_BEGIN);
				}
				else{
					changeState(GAME_QUIT);
				}
			}
		}
	}
}

void GameEngine::deleteFlagged(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (vector<CompositeEntity*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
			if ((*itr2)->getDoDelete()){
				vector<CompositeEntity*>::iterator newItr = itr->second.erase(itr2);
				itr2 = newItr;
				if (itr2 == itr->second.end()){
					break;
				}
			}
		}
	}

	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntitiesRenderingOrder[currentLevel].begin(); itr != gameEntitiesRenderingOrder[currentLevel].end(); itr++){
		for (vector<CompositeEntity*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
			if ((*itr2)->getDoDelete()){
				delete (*itr2);
				(*itr2) = nullptr;
				vector<CompositeEntity*>::iterator newItr = itr->second.erase(itr2);
				itr2 = newItr;
				if (itr2 == itr->second.end()){
					break;
				}
			}
		}
	}
}

void GameEngine::moveAll(unsigned entityType, float x, float y, float z){
	for (CompositeEntity* entity : gameEntities[currentLevel][entityType]){
		float posX = entity->getPosition().x + x;
		float posY = entity->getPosition().y + y;
		float posZ = entity->getPosition().z + z;

		entity->setPosition(posX, posY, posZ);
	}
}

void GameEngine::turnAll(unsigned entityType, bool turnX, bool turnY, bool turnZ){
	for (CompositeEntity* entity : gameEntities[currentLevel][entityType]){
		float velX = turnX ? -entity->getVelocity().x : entity->getVelocity().x;
		float velY = turnY ? -entity->getVelocity().y : entity->getVelocity().y;
		float velZ = turnZ ? -entity->getVelocity().z : entity->getVelocity().z;

		entity->setVelocity(velX, velY, velZ);
	}
}

void GameEngine::run(float elapsed, const Uint8* input, SDL_Event input2, ShaderProgram* program){
	handleInput(input, input2);
	float fixedElapsed = elapsed;
	if (fixedElapsed >= FIXED_TIMESTEP * MAX_TIMESTEP){
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEP;
	}

	while (fixedElapsed >= FIXED_TIMESTEP){
		fixedElapsed -= FIXED_TIMESTEP;
		update(FIXED_TIMESTEP, program);
	}
	update(fixedElapsed, program);
	render(program, elapsed, FRAMES_PER_SECOND);
}

void GameEngine::update(float elapsed, ShaderProgram* program){
	unsigned enemiesKilled = 0;
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			if (entity->getIsActive()){
				entity->move(elapsed, GRAVITY);
				if (entity->atScreenBoundary(0, mapWidth, -(int)(mapHeight), 0)){
					entity->boundaryAction(0, mapWidth, -(int)(mapHeight), 0);
				}
				CompositeEntity* lastProjectile = gameEntities[currentLevel][ENEMY_PROJECTILE].size() != 0 ? gameEntities[currentLevel][ENEMY_PROJECTILE][gameEntities[currentLevel][ENEMY_PROJECTILE].size() - 1] : nullptr;
				CompositeEntity* projectile = entity->getIsActive() ? entity->logic(playerEntity, lastProjectile) : nullptr;
				if (projectile != nullptr){
					addGameEntity(currentLevel, projectile);
				}
			}
			entity->addToTimeSinceFiring(elapsed);
			if ((entity->getEntityType() == ACTOR_ENEMY || entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN) && gameState == GAME_PLAY){
				if (!entity->getIsActive() && (entity->getPosition().x != 0 && entity->getPosition().y != 0 && entity->getPosition().z != 1)){
					enemiesKilled++;
					entity->setPosition(0, 0, 1);
					points += 100;
				}
			}

			if (entity->getEntityType() == ENTITY_COIN && gameState == GAME_PLAY){
				if (!entity->getIsActive() && (entity->getPosition().x != 0 && entity->getPosition().y != 0 && entity->getPosition().z != 1)){
					entity->setPosition(0, 0, 0);
					points += 10;
				}
			}
			

			if (entity->getEntityType() == POINTS_INDICATOR && entity->getIsActive()){
				entity->setDisplayText("Points: " + std::to_string(points));
			}

			entity->runAnimation(elapsed, FRAMES_PER_SECOND);
		}
	}

	if (gameState == GAME_PLAY){
		for (CompositeEntity* entity : gameEntities[currentLevel][ACTOR_ENEMY]){
			if (entity->getIsActive() && entity->atScreenBoundary(0, mapWidth, -(int)(mapHeight), 0)){
				float moveX = entity->getPosition().x > 0 ? -0.02 : 0.02;
				moveAll(ACTOR_ENEMY, moveX, -0.125);
				turnAll(ACTOR_ENEMY, true, false);
				break;
			}
		}
	}

	deleteFlagged();

	collisionCheck(elapsed);
	checkTileCollisions();

	if (gameState == GAME_PLAY && !playerEntity->getIsActive()){
		if (lives > 1){
			lives--;
			//gameEntities[currentLevel][LIFE_ICON_ENTITY][lives]->setIsActive(false);
			playerEntity->setIsActive(true);
			blinkCount = 0;
			blinkTime = BLINK_TIMING;
			playerEntity->setisInvincible(true);
			playerEntity->resetToCheckpoint();
		}
		else{
			changeState(GAME_END);
		}
	}

	if (blinkCount <= MAX_BLINK_COUNT && gameState == GAME_PLAY){
		if (blinkTime >= BLINK_TIMING){
			blinkTime = 0;
			blinkCount++;
			playerEntity->blink();
		}
		blinkTime += elapsed;
	}
	else if (blinkCount > MAX_BLINK_COUNT && gameState == GAME_PLAY){
		playerEntity->setisInvincible(false);
	}

	gameOver = gameState == GAME_QUIT;
	checkIfShouldWarp();
}

void GameEngine::changeTitleSelection(){
	if (gameState == TITLE_SCREEN){
		for (CompositeEntity* entity : gameEntities[currentLevel][TITLE_TEXT_ENTITY]){
				if (entity->getEntityID() == "Selection01"){
					if (titleSelection == 0){
						entity->setDisplayText(">> Begin Game!");
					}
					else{
						entity->setDisplayText("Begin Game!");
					}
				}

				if (entity->getEntityID() == "Selection02"){
					if (titleSelection == 1){
						entity->setDisplayText(">> Quit");
					}
					else{
						entity->setDisplayText("Quit");
					}
				}
			}
		}
}

void GameEngine::deActivateLevelEntities(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			entity->reset();
			if (entity->getEntityType() != POINTS_INDICATOR){
				entity->setIsActive(false);
			}
		}
	}
}

void GameEngine::activateLevelEntities(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			if (entity != nullptr){
				entity->reset();
				if (entity->getEntityType() != ACTOR_PLAYER && entity->getEntityType() != ACTOR_ENEMY && entity->getEntityType() != GAME_TEXT_ENTITY && entity->getEntityType() != ICON_ENTITY && entity->getEntityType() != POINTS_INDICATOR && entity->getEntityType() != LIFE_ICON_ENTITY && entity->getEntityType() != STATIC_ENTITY && entity->getEntityType() != ACTOR_ENEMY_PATROL_TURN && entity->getEntityType() != ENTITY_COIN){
					entity->setIsActive(false);
				}
				else{
					entity->setIsActive(true);
					entity->setAllRender(true);
					if (entity->getEntityType() == POINTS_INDICATOR){
						entity->setDisplayText(entity->getDisplayText() + ' ' + to_string(points));
					}
					entity->updateBounding();
				}
			}
		}
	}
}

void GameEngine::changeState(unsigned state){
	switch (state){
	case TITLE_SCREEN:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
			for (CompositeEntity* entity : itr->second){
				entity->reset();
				entity->setIsActive(entity->getEntityType() == TITLE_TEXT_ENTITY);
			}
		}
		gameState = TITLE_SCREEN;
		changeTitleSelection();
		break;
	case GAME_BEGIN:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
			for (CompositeEntity* entity : itr->second){
				if (entity != nullptr){
					if (entity->getEntityType() != ACTOR_PLAYER && entity->getEntityType() != ACTOR_ENEMY && entity->getEntityType() != GAME_TEXT_ENTITY && entity->getEntityType() != ICON_ENTITY && entity->getEntityType() != POINTS_INDICATOR && entity->getEntityType() != LIFE_ICON_ENTITY && entity->getEntityType() != STATIC_ENTITY && entity->getEntityType() != ACTOR_ENEMY_PATROL_TURN && entity->getEntityType() != ENTITY_COIN && entity->getEntityType() != WARP_ENTITY){
						entity->setIsActive(false);
					}
					else{
						entity->reset();
						entity->setIsActive(true);
						if (entity->getEntityType() == POINTS_INDICATOR){
							entity->setDisplayText(entity->getDisplayText() + ' ' + to_string(points));
						}
						entity->updateBounding();
					}
				}
			}
		}
		playerEntity->getEntities()->setDoRender(true);
		gameState = GAME_PLAY;
		lives = 3;
		blinkCount = MAX_BLINK_COUNT + 1;
		points = 0;
		break;
	case GAME_PLAY:
		break;
	case GAME_END:
		changeState(TITLE_SCREEN);
		break;
	case GAME_QUIT:
		break;
	default:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
			for (CompositeEntity* entity : itr->second){
				if (entity->getEntityType() != TITLE_TEXT_ENTITY){
					entity->setIsActive(false);
				}
				else{
					entity->reset();
					entity->setIsActive(true);
				}
			}
		}
		gameState = TITLE_SCREEN;
		changeTitleSelection();
		break;
		
	}
}

bool GameEngine::getGameOver(){
	return gameOver;
}

void GameEngine::start(){
	changeState(TITLE_SCREEN);
}

void GameEngine::checkIfShouldWarp(){
	for (CompositeEntity* entity : gameEntities[currentLevel][WARP_ENTITY]){
		if (playerEntity->isColliding(entity)){
			setLevel(entity->getWarpDestination());
		}
	}
}