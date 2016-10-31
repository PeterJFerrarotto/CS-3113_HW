#include "GameEngine.h"
#include "CompositeEntity.h"
#include "CollisionListener.h"
#include "Texture.h"
#include "Entity.h"
#include "ShaderProgram.h"
using namespace std;

GameEngine::GameEngine() : gameOver(false), titleSelection(0)
{
	changeState(TITLE_SCREEN);
	enemyCount = 0;
}

GameEngine::~GameEngine()
{
}

void GameEngine::render(ShaderProgram* program){
	if (backGroundTexture != nullptr){
		vector<GLfloat> textureCoordinates = backGroundTexture->getTextureCoordinates();
		glBindTexture(GL_TEXTURE_2D, backGroundTexture->getTextureID());
		Matrix backGroundMatrix;
		program->setModelMatrix(backGroundMatrix);

		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, backGroundVertices.data());

		glEnableVertexAttribArray(program->texCoordAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
	for (size_t i = 0; i < gameEntitiesRenderingOrder.size(); i++){
		for (vector<CompositeEntity*>::const_iterator itr = gameEntitiesRenderingOrder[i].begin(); itr != gameEntitiesRenderingOrder[i].end(); itr++){
			if ((*itr)->getIsActive())
				(*itr)->draw(program);
		}
	}
}

void GameEngine::addGameEntity(CompositeEntity* entity){
	if (entity->getEntityType() != BACKGROUND_ENTITY){
		gameEntities[entity->getEntityType()].push_back(entity);
		gameEntitiesRenderingOrder[entity->getLayer()].push_back(entity);
		if (entity->getEntityType() == ACTOR_PLAYER){
			playerEntity = entity;
		}
		if (entity->getEntityType() == ACTOR_ENEMY){
			enemyCount++;
		}
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

void GameEngine::setBackGroundTexture(Texture* backGroundTexture){
	this->backGroundTexture = backGroundTexture;
}

void GameEngine::checkStaticCollisions(){
	for (size_t i = 0; i < gameEntities.size(); i++){
		for (CompositeEntity* entity : gameEntities[i]){
			if (entity->getEntityType() == STATIC_ENTITY){
				return;
			}
			if (entity->getCanCollide() && entity->getIsActive()){
				for (CompositeEntity* staticEntity : gameEntities[STATIC_INDEX]){
					if (staticEntity->getIsActive() && staticEntity->getCanCollide()){
						if (entity->isColliding(staticEntity)){
							float penetrationY = fabs(fabs(entity->getPosition().y - staticEntity->getPosition().y) - entity->getTotalBounding().y - staticEntity->getTotalBounding().y);
							entity->collideWithStatic(penetrationY, Y);
							float penetrationX = fabs(fabs(entity->getPosition().x - staticEntity->getPosition().x) - entity->getTotalBounding().x - staticEntity->getTotalBounding().x);
							entity->collideWithStatic(penetrationX, X);
						}
					}
				}
			}
		}
	}
}

void GameEngine::handleInput(const Uint8* input, SDL_Event input2){
	if (input[SDL_SCANCODE_ESCAPE]){
		gameState = GAME_QUIT;
	}

	if (input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getIsActive()){
			if (playerEntity->getVelocity().x <= -PLAYER_TOP_SPEED){
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setVelocity(-PLAYER_TOP_SPEED, playerEntity->getVelocity().y);
			}
			else{
				playerEntity->setAcceleration(-PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
			}
		}
	}

	if (input[SDL_SCANCODE_RIGHT] && !input[SDL_SCANCODE_LEFT]){
		if (playerEntity->getIsActive()){
			if (playerEntity->getVelocity().x >= PLAYER_TOP_SPEED){
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setVelocity(PLAYER_TOP_SPEED, playerEntity->getVelocity().y);
			}
			else{
				playerEntity->setAcceleration(PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
			}
		}
	}

	if (!input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getIsActive()){
			if (!playerEntity->atScreenBoundary(GAME_WALL, GAME_CEILING)){
				if (playerEntity->getVelocity().x > 0){
					playerEntity->setAcceleration(-PLAYER_ACCELERATION / 2, playerEntity->getAcceleration().y / 2);
				}
				else if (playerEntity->getVelocity().x < 0){
					playerEntity->setAcceleration(PLAYER_ACCELERATION / 2, playerEntity->getAcceleration().y / 2);
				}
			}
		}
	}

	if (input[SDL_SCANCODE_UP]){
		if (gameState == GAME_PLAY && playerEntity->getIsActive()){
			if (playerEntity->getIsActive() && playerEntity->getOnGround()){
				playerEntity->setVelocity(playerEntity->getVelocity().x, 2.0);
			}
		}
	}

	if (input[SDL_SCANCODE_SPACE]){
		if (gameState == GAME_PLAY && playerEntity->getIsActive()){
			CompositeEntity* projectile = playerEntity->fire();
			if (projectile != nullptr){
				addGameEntity(projectile);
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
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
		for (vector<CompositeEntity*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
			if ((*itr2)->getDoDelete()){
				if ((*itr2)->getEntityType() == ACTOR_ENEMY_SAUCER){
					points += 250;
				}
				vector<CompositeEntity*>::iterator newItr = itr->second.erase(itr2);
				itr2 = newItr;
				if (itr2 == itr->second.end()){
					break;
				}
			}
		}
	}

	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntitiesRenderingOrder.begin(); itr != gameEntitiesRenderingOrder.end(); itr++){
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
	for (CompositeEntity* entity : gameEntities[entityType]){
		float posX = entity->getPosition().x + x;
		float posY = entity->getPosition().y + y;
		float posZ = entity->getPosition().z + z;

		entity->setPosition(posX, posY, posZ);
	}
}

void GameEngine::turnAll(unsigned entityType, bool turnX, bool turnY, bool turnZ){
	for (CompositeEntity* entity : gameEntities[entityType]){
		float velX = turnX ? -entity->getVelocity().x : entity->getVelocity().x;
		float velY = turnY ? -entity->getVelocity().y : entity->getVelocity().y;
		float velZ = turnZ ? -entity->getVelocity().z : entity->getVelocity().z;

		entity->setVelocity(velX, velY, velZ);
	}
}

void GameEngine::updateEntities(float elapsed, const Uint8* input, SDL_Event input2, ShaderProgram* program){
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
}

void GameEngine::update(float elapsed, ShaderProgram* program){
	unsigned enemiesKilled = 0;
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
		for (CompositeEntity* entity : itr->second){
			entity->move(elapsed, GRAVITY);
			if (entity->atScreenBoundary(GAME_WALL, GAME_CEILING)){
				entity->boundaryAction(GAME_WALL, GAME_CEILING);
			}
			entity->addToTimeSinceFiring(elapsed);
			CompositeEntity* lastProjectile = gameEntities[ENEMY_PROJECTILE].size() != 0 ? gameEntities[ENEMY_PROJECTILE][gameEntities[ENEMY_PROJECTILE].size() - 1] : nullptr;
			CompositeEntity* projectile = entity->getIsActive() ? entity->logic(playerEntity, lastProjectile) : nullptr;
			if (projectile != nullptr){
				addGameEntity(projectile);
			}
			if (entity->getEntityType() == ACTOR_ENEMY && gameState == GAME_PLAY){
				if (!entity->getIsActive()){
					enemiesKilled++;
				}
			}
			points = enemiesKilled * 50;

			if (entity->getEntityType() == POINTS_INDICATOR && entity->getIsActive()){
				entity->setDisplayText("Points: " + std::to_string(points));
			}
		}
	}

	if (enemiesKilled == enemyCount && gameState == GAME_PLAY){
		changeState(GAME_END);
	}

	if (gameState == GAME_PLAY){
		for (CompositeEntity* entity : gameEntities[ACTOR_ENEMY]){
			if (entity->getIsActive() && entity->atScreenBoundary(GAME_WALL, GAME_CEILING)){
				float moveX = entity->getPosition().x > 0 ? -0.02 : 0.02;
				moveAll(ACTOR_ENEMY, moveX, -0.125);
				turnAll(ACTOR_ENEMY, true, false);
				break;
			}
		}
	}

	deleteFlagged();

	collisionCheck(elapsed);
	checkStaticCollisions();

	if (gameState == GAME_PLAY && !playerEntity->getIsActive()){
		if (lives > 1){
			lives--;
			gameEntities[LIFE_ICON_ENTITY][lives]->setIsActive(false);
			playerEntity->setIsActive(true);
			blinkCount = 0;
			blinkTime = BLINK_TIMING;
			playerEntity->setisInvincible(true);
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

	render(program);
	gameOver = gameState == GAME_QUIT;
}

void GameEngine::changeTitleSelection(){
	if (gameState == TITLE_SCREEN){
			for (CompositeEntity* entity : gameEntities[TITLE_TEXT_ENTITY]){
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

void GameEngine::changeState(unsigned state){
	switch (state){
	case TITLE_SCREEN:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
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
	case GAME_BEGIN:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
			for (CompositeEntity* entity : itr->second){
				if (entity->getEntityType() != ACTOR_PLAYER && entity->getEntityType() != ACTOR_ENEMY && entity->getEntityType() != GAME_TEXT_ENTITY && entity->getEntityType() != ICON_ENTITY && entity->getEntityType() != POINTS_INDICATOR && entity->getEntityType() != LIFE_ICON_ENTITY && entity->getEntityType() != STATIC_ENTITY){
					entity->setIsActive(false);
				}
				else{
					entity->reset();
					entity->setIsActive(true);
					if (entity->getEntityType() == POINTS_INDICATOR){
						entity->setDisplayText(entity->getDisplayText() + ' ' + to_string(points));
					}
				}
				if (entity != nullptr){
					entity->updateBounding();
				}
			}
		}
		playerEntity->getEntities()->setDoRender(true);
		gameState = GAME_PLAY;
		lives = 3;
		blinkCount = MAX_BLINK_COUNT + 1;
		break;
	case GAME_PLAY:
		break;
	case GAME_END:
		changeState(TITLE_SCREEN);
		break;
	case GAME_QUIT:
		break;
	default:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
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