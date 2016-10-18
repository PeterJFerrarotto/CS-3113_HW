#include "GameEngine.h"
#include "CompositeEntity.h"
#include "CollisionListener.h"
#include "Texture.h"
using namespace std;

GameEngine::GameEngine(Texture* projectileTexture) : gameOver(false), projectileTexture(projectileTexture)
{
}

GameEngine::~GameEngine()
{
}

void GameEngine::render(ShaderProgram* program){
	for (size_t i = 0; i < gameEntities.size(); i++){
		for (vector<CompositeEntity*>::const_iterator itr = gameEntities[i].begin(); itr != gameEntities[i].end(); itr++){
			if ((*itr)->getIsActive())
				(*itr)->draw(program);
		}
	}
}

void GameEngine::addGameEntity(CompositeEntity* entity){
	gameEntities[entity->getEntityType()].push_back(entity);
	if (entity->getEntityType() == ACTOR_PLAYER){
		playerEntity = entity;
	}
}

void GameEngine::addCollisionEvent(CollisionListener* collisionEvent){
	collisionEvents.push_back(collisionEvent);
}

void GameEngine::collisionCheck(float elapsed){
	for (size_t i = 0; i < collisionEvents.size(); i++){
		(*collisionEvents[i])(elapsed, gameEntities);
	}
}

void GameEngine::handleInput(const Uint8* input){
	gameOver = input[SDL_SCANCODE_ESCAPE];

	if (input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getVelocity().x <= -PLAYER_TOP_SPEED){
			playerEntity->setAcceleration(0, 0);
			playerEntity->setVelocity(-PLAYER_TOP_SPEED, 0);
		}
		else{
			playerEntity->setAcceleration(-PLAYER_ACCELERATION, 0);
		}
	}

	if (input[SDL_SCANCODE_RIGHT] && !input[SDL_SCANCODE_LEFT]){
		if (playerEntity->getVelocity().x >= PLAYER_TOP_SPEED){
			playerEntity->setAcceleration(0, 0);
			playerEntity->setVelocity(PLAYER_TOP_SPEED, 0);
		}
		else{
			playerEntity->setAcceleration(PLAYER_ACCELERATION, 0);
		}
	}

	if (!input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (!playerEntity->atScreenBoundary(GAME_WALL, GAME_CEILING)){
			if (playerEntity->getVelocity().x > 0){
				playerEntity->setAcceleration(-PLAYER_ACCELERATION / 2, 0);
			}
			else if (playerEntity->getVelocity().x < 0){
				playerEntity->setAcceleration(PLAYER_ACCELERATION / 2, 0);
			}
		}
	}

	if (input[SDL_SCANCODE_SPACE]){
		CompositeEntity* projectile = playerEntity->fire(projectileTexture);
		if (projectile != nullptr){
			addGameEntity(projectile);
		}
	}
}

void GameEngine::deleteFlagged(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
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
}

void GameEngine::updateEntities(float elapsed, const Uint8* input, ShaderProgram* program){
	handleInput(input);
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
		for (CompositeEntity* entity : itr->second){
			entity->move(elapsed);
			if (entity->atScreenBoundary(GAME_WALL, GAME_CEILING)){
				entity->boundaryAction(GAME_WALL, GAME_CEILING);
			}
			entity->addToTimeSinceFiring(elapsed);
			CompositeEntity* lastProjectile = gameEntities[ENEMY_PROJECTILE].size() != 0 ? gameEntities[ENEMY_PROJECTILE][gameEntities[ENEMY_PROJECTILE].size() - 1] : nullptr;
			CompositeEntity* projectile = entity->logic(playerEntity, projectileTexture, lastProjectile);
			if (projectile != nullptr){
				addGameEntity(projectile);
			}
		}
	}

	deleteFlagged();

	collisionCheck(elapsed);

	render(program);

	gameOver = !playerEntity->getIsActive();
}

bool GameEngine::getGameOver(){
	return gameOver;
}