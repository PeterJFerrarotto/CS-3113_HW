#include "CompositeEntity.h"
#include "Entity.h"

CompositeEntity::CompositeEntity()
{
}

CompositeEntity::CompositeEntity(Entity* first){
	this->first = first;
	updateBounding();
}

CompositeEntity::~CompositeEntity()
{
}


Entity* CompositeEntity::getEntities(){
	return first;
}

Vector3 CompositeEntity::getPosition(){
	return position;
}

Vector3 CompositeEntity::getVelocity(){
	return velocity;
}

Vector3 CompositeEntity::getAcceleration(){
	return acceleration;
}

Vector3 CompositeEntity::getTotalBounding(){
	return totalBounding;
}

Vector3 CompositeEntity::getScale(){
	return scale;
}

int CompositeEntity::getBoundingType(){
	return boundingType;
}

float CompositeEntity::getRotation(){
	return rotation;
}

float CompositeEntity::getRotationalVelocity(){
	return rotationalVelocity;
}

int CompositeEntity::getEntityType(){
	return type;
}

unsigned CompositeEntity::getCollisionBehavior(){
	return collisionBehavior;
}

unsigned CompositeEntity::getBoundaryBehavior(){
	return boundaryBehavior;
}

unsigned CompositeEntity::getState(){
	return state;
}

bool CompositeEntity::getIsActive(){
	return isActive;
}

bool CompositeEntity::getDoDelete(){
	return doDelete;
}

bool CompositeEntity::getCanCollide(){
	return canCollide;
}


void CompositeEntity::setEntities(Entity* first){
	this->first = first;
	updateBounding();
}

void CompositeEntity::setPosition(float x, float y, float z){
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}

void CompositeEntity::setVelocity(float x, float y, float z){
	this->velocity.x = x;
	this->velocity.y = y;
	this->velocity.z = z;
}

void CompositeEntity::setAcceleration(float x, float y, float z){
	this->acceleration.x = x;
	this->acceleration.y = y;
	this->acceleration.z = z;
}

void CompositeEntity::setScale(float x, float y, float z){
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

void CompositeEntity::setBoundingType(int boundingType){
	this->boundingType = boundingType;
}

void CompositeEntity::setRotation(float rotation){
	this->rotation = rotation;
}

void CompositeEntity::setRotationalVelocity(float rotationalVelocity){
	this->rotationalVelocity = rotationalVelocity;
}

void CompositeEntity::setEntityType(int type){
	this->type = type;
}

void CompositeEntity::setCollisionBehavior(unsigned behavior){
	this->collisionBehavior = behavior;
}

void CompositeEntity::setBoundaryBehavior(unsigned behavior){
	boundaryBehavior = behavior;
}

void CompositeEntity::setState(unsigned state){
	this->state = state;
}

void CompositeEntity::setIsActive(bool isActive){
	this->isActive = isActive;
}

void CompositeEntity::setCanCollide(bool canCollide){
	this->canCollide = canCollide;
}

void CompositeEntity::addEntity(Entity* toAdd){
	first->addEntity(toAdd);
	updateBounding();
}

void CompositeEntity::updateBounding(){
	Entity* checking = first;
	if (boundingType == SQUARE){
		checking->updateBounding();
		totalBounding.x = first->getBounding().x * scale.x;
		totalBounding.y = first->getBounding().y * scale.y;
		totalBounding.z = first->getBounding().z * scale.z;
		checking = checking->getNext();
		while (checking != nullptr){
			checking->updateBounding();
			if ((abs(checking->getPosition().x) + checking->getBounding().x) * scale.x > totalBounding.x){
				totalBounding.x = (abs(checking->getPosition().x) + checking->getBounding().x) * scale.x;
			}

			if ((abs(checking->getPosition().y) + checking->getBounding().y) * scale.y > totalBounding.y){
				totalBounding.y = (abs(checking->getPosition().y) + checking->getBounding().y) * scale.y;
			}

			if ((abs(checking->getPosition().z) + checking->getBounding().z) * scale.z > totalBounding.z){
				totalBounding.z = (abs(checking->getPosition().z) + checking->getBounding().z) * scale.z;
			}
		}
	}
}

bool CompositeEntity::subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat){
	if (firstOfThis->getCanCollide() && firstOfThat->getCanCollide()){
		if (firstOfThis->getBoundingType() == SQUARE && firstOfThat->getBoundingType() == SQUARE){
			if (!(firstOfThis->getPosition().x + firstOfThis->getBounding().x < firstOfThat->getPosition().x - firstOfThat->getBounding().x || firstOfThis->getPosition().x - totalBounding.x > firstOfThat->getPosition().x + firstOfThat->getBounding().x || firstOfThis->getPosition().y + firstOfThis->getBounding().y < firstOfThat->getPosition().y - firstOfThat->getBounding().y || firstOfThis->getPosition().y - firstOfThis->getBounding().y > firstOfThat->getPosition().y + firstOfThat->getBounding().y)){
				return true;
			}
		}
		//PLACEHOLDER
		return false;
	}
	if (firstOfThat->getNext() != nullptr){
		return subEntitiesColliding(firstOfThis, firstOfThat->getNext(), originalOfThat);
	}
	else{
		if (firstOfThis->getNext() != nullptr){
			return subEntitiesColliding(firstOfThis->getNext(), originalOfThat, originalOfThat);
		}
	}
	
	return false;
}

bool CompositeEntity::atScreenBoundary(float gameWall, float gameCeiling){
	bool atBoundary = (abs(position.x) + totalBounding.x >= gameWall || abs(position.y) + totalBounding.y >= gameCeiling);
	if (atBoundary){
		boundaryLeft = position.x - totalBounding.x <= -gameWall;
		boundaryRight = position.x + totalBounding.x >= gameWall;
		boundaryTop = position.y + totalBounding.y >= gameWall;
		boundaryBottom = position.y - totalBounding.y <= -gameWall;
	}
	else{
		boundaryLeft = false;
		boundaryRight = false;
		boundaryTop = false;
		boundaryBottom = false;
	}

	return atBoundary;
}

bool CompositeEntity::isColliding(CompositeEntity* collidingWith){
	updateBounding();
	if (canCollide && collidingWith->getCanCollide() && isActive && collidingWith->getIsActive()){
		if (boundingType == SQUARE && collidingWith->boundingType == SQUARE){
			if (!(position.x + totalBounding.x < collidingWith->position.x - collidingWith->totalBounding.x || position.x - totalBounding.x > collidingWith->position.x + collidingWith->totalBounding.x || position.y + totalBounding.y < collidingWith->position.y - collidingWith->totalBounding.y || position.y - totalBounding.y > collidingWith->position.y + collidingWith->totalBounding.y)){
				bool isColliding = subEntitiesColliding(first, collidingWith->first, collidingWith->first);
				if (isColliding){
					collideLeft = position.x - totalBounding.x < collidingWith->position.x + collidingWith->totalBounding.x;
					collideRight = position.x + totalBounding.x > collidingWith->position.x - collidingWith->totalBounding.x;
					collideBottom = position.y - totalBounding.y < collidingWith->position.y + collidingWith->totalBounding.y;
					collideTop = position.y + totalBounding.y > collidingWith->position.y - collidingWith->totalBounding.y;

					if (collideLeft && collideRight){
						collideRight = false;
					}
					if (collideTop && collideBottom){
						collideTop = false;
					}
				}
				else{
					collideLeft = false;
					collideRight = false;
					collideTop = false;
					collideBottom = false;
				}
				return isColliding;
			}
			return false;
		}
		return false;
	}
	return false;
}


void CompositeEntity::move(float elapsed){
	if (state == BOUNCING){

	}

	else if (state == BOUNCED){
		velocity.x = 0;
		velocity.y = 0;
		velocity.z = 0;
		state = STATIONARY;
	}

	else{
		position.x += velocity.x * elapsed;
		position.y += velocity.y * elapsed;
		position.z += velocity.z * elapsed;

		velocity.x += acceleration.x * elapsed;
		velocity.y += acceleration.y * elapsed;
		velocity.z += acceleration.z * elapsed;

		rotation += rotationalVelocity * elapsed;
		first->move(elapsed);

		if (velocity.x != 0 || velocity.y != 0 || velocity.z != 0){
			setState(MOVING);
		}

		if (acceleration.x != 0 || acceleration.y != 0 || acceleration.z != 0){
			setState(ACCELERATING);
		}
	}
}

void CompositeEntity::draw(ShaderProgram* program){
	first->draw(program, position, scale, rotation);
}

void CompositeEntity::bounce(float elapsed, CompositeEntity* bouncingOffOf){
	if (collideLeft){
		position.x = bouncingOffOf->position.x + totalBounding.x;
		if (velocity.x < 0){
			velocity.x *= -0.8;
		}
		collideLeft = velocity.x == 0;
	}
	if (collideRight){
		position.x = bouncingOffOf->position.x - totalBounding.x;
		if (velocity.x > 0){
			velocity.x *= -0.8;
		}
		collideRight = velocity.x == 0;
	}
	if (collideTop){
		position.y = bouncingOffOf->position.y - totalBounding.y;
		if (velocity.y > 0){
			velocity.y *= -0.8;
		}
		collideTop = velocity.y == 0;
	}
	if (collideBottom){
		position.y = bouncingOffOf->position.y + totalBounding.y;
		if (velocity.y < 0){
			velocity.y *= -0.8;
		}
		collideBottom = velocity.y == 0;
	}

	state = BOUNCED;
}

void CompositeEntity::stop(){
	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;

	acceleration.x = 0;
	acceleration.y = 0;
	acceleration.z = 0;

	state = STATIONARY;
}

void CompositeEntity::destroy(){
	doDelete = true;
}

void CompositeEntity::deActivate(){
	isActive = false;
}


void CompositeEntity::boundaryStop(float gameWall, float gameCeiling){
	if ((boundaryLeft && velocity.x < 0) || (boundaryRight && velocity.x > 0)){
		velocity.x = 0;
		acceleration.x = 0;


		if (boundaryLeft && position.x - totalBounding.x != -gameWall){
			position.x = -gameWall + totalBounding.x;
		}

		if (boundaryRight && position.x + totalBounding.x != gameWall){
			position.x = gameWall - totalBounding.x;
		}
	}

	if (boundaryTop || boundaryBottom){
		velocity.y = 0;
		acceleration.y = 0;
	}

	if (boundaryBottom && position.y - totalBounding.y != -gameWall){
		position.y = -gameWall + totalBounding.y;
	}

	if (boundaryTop && position.y + totalBounding.y != gameWall){
		position.y = gameWall - totalBounding.y;
	}
	state = STATIONARY;
}

void CompositeEntity::boundaryTurn(float gameWall, float gameCeiling){
	if (boundaryLeft || boundaryRight){
		velocity.x = -velocity.x;
		acceleration.x = -acceleration.x;
	}

	if (boundaryLeft && position.x - totalBounding.x != -gameWall){
		position.x = -gameWall + totalBounding.x + 0.01;
		boundaryLeft = false;
	}

	if (boundaryRight && position.x + totalBounding.x != gameWall){
		position.x = gameWall - totalBounding.x - 0.01;
		boundaryRight = false;
	}

	if (boundaryTop || boundaryBottom){
		velocity.y = -velocity.y;
		acceleration.y = -acceleration.y;
	}

	if (boundaryBottom && position.y - totalBounding.y != -gameWall){
		position.y = -gameWall + totalBounding.y + 0.01;
		boundaryBottom = false;
	}

	if (boundaryTop && position.y + totalBounding.y != gameWall){
		position.y = gameWall - totalBounding.y - 0.01;
		boundaryTop = false;
	}
}

void CompositeEntity::boundaryTurnAndDown(float gameWall, float gameCeiling){
	boundaryTurn(gameWall, gameCeiling);
	position.y -= 0.125;
}

//Throws exception if collision behavior unrecognized
void CompositeEntity::collide(float elapsed, CompositeEntity* bouncingOffOf){
	switch (collisionBehavior){
	case BOUNCE:
		state = BOUNCING;
		bounce(elapsed, bouncingOffOf);
		break;
	case DESTROY:
		destroy();
		break;
	case DEACTIVATE:
		deActivate();
		break;
	default:
		throw "UNRECOGNIZED COLLISION BEHAVIOR!";
		break;
	}
}

//Throws exception if boundary behavior unrecognized
void CompositeEntity::boundaryAction(float gameWall, float gameCeiling){
	switch (boundaryBehavior){
	case BOUND_BOUNCE:
		break;
	case BOUND_TURN:
		boundaryTurn(gameWall, gameCeiling);
		break;
	case BOUND_STOP:
		boundaryStop(gameWall, gameCeiling);
		break;
	case BOUND_DESTROY:
		destroy();
		break;
	case BOUND_DEACTIVATE:
		deActivate();
		break;
	case BOUND_TURN_AND_DOWN:
		boundaryTurnAndDown(gameWall, gameCeiling);
		break;
	case BOUND_NOTHING:
		break;
	default:
		throw "UNKNOWN BOUNDARY BEHAVIOR!";
		break;
	}
}

CompositeEntity* CompositeEntity::fire(Texture* projectileTexture){
	return nullptr;
}

void CompositeEntity::addToTimeSinceFiring(float elapsed){
	return;
}

CompositeEntity* CompositeEntity::logic(CompositeEntity* player, Texture* projectileTexture, CompositeEntity* lastProjectile){
	return nullptr;
}