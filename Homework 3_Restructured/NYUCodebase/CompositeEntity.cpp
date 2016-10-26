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
	if (first != nullptr){
		first->freeMemory();
	}
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

float CompositeEntity::getRotation(){
	return rotation;
}

float CompositeEntity::getRotationalVelocity(){
	return rotationalVelocity;
}

BOUNDING_TYPE CompositeEntity::getBoundingType(){
	return boundingType;
}

ENTITY_TYPE CompositeEntity::getEntityType(){
	return type;
}

COLLISION_BEHAVIOR CompositeEntity::getCollisionBehavior(){
	return collisionBehavior;
}

BOUNDARY_BEHAVIOR CompositeEntity::getBoundaryBehavior(){
	return boundaryBehavior;
}

STATE CompositeEntity::getState(){
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

bool CompositeEntity::getisInvincible(){
	return isInvincible;
}

const std::string& CompositeEntity::getEntityID(){
	return entityID;
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

void CompositeEntity::setStartingPosition(float x, float y, float z){
	this->startingPosition.x = x;
	this->startingPosition.y = y;
	this->startingPosition.z = z;
}

void CompositeEntity::setVelocity(float x, float y, float z){
	this->velocity.x = x;
	this->velocity.y = y;
	this->velocity.z = z;
}

void CompositeEntity::setStartingVelocity(float x, float y, float z){
	this->startingVelocity.x = x;
	this->startingVelocity.y = y;
	this->startingVelocity.z = z;
}

void CompositeEntity::setAcceleration(float x, float y, float z){
	this->acceleration.x = x;
	this->acceleration.y = y;
	this->acceleration.z = z;
}

void CompositeEntity::setStartingAcceleration(float x, float y, float z){
	this->startingAcceleration.x = x;
	this->startingAcceleration.y = y;
	this->startingAcceleration.z = z;
}

void CompositeEntity::setScale(float x, float y, float z){
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

void CompositeEntity::setStartingScale(float x, float y, float z){
	this->startingScale.x = x;
	this->startingScale.y = y;
	this->startingScale.z = z;
}

void CompositeEntity::setRotation(float rotation){
	this->rotation = rotation;
}

void CompositeEntity::setRotationalVelocity(float rotationalVelocity){
	this->rotationalVelocity = rotationalVelocity;
}

void CompositeEntity::setBoundingType(BOUNDING_TYPE boundingType){
	this->boundingType = boundingType;
}

void CompositeEntity::setEntityType(ENTITY_TYPE type){
	this->type = type;
}

void CompositeEntity::setCollisionBehavior(COLLISION_BEHAVIOR behavior){
	this->collisionBehavior = behavior;
}

void CompositeEntity::setBoundaryBehavior(BOUNDARY_BEHAVIOR behavior){
	boundaryBehavior = behavior;
}

void CompositeEntity::setState(STATE state){
	this->state = state;
}

void CompositeEntity::setIsActive(bool isActive){
	this->isActive = isActive;
}

void CompositeEntity::setCanCollide(bool canCollide){
	this->canCollide = canCollide;
}

void CompositeEntity::setEntityID(const std::string& entityID){
	this->entityID = entityID;
}

void CompositeEntity::setisInvincible(bool isInvincible){
	this->isInvincible = isInvincible;
}


void CompositeEntity::reset(){
	position.x = startingPosition.x;
	position.y = startingPosition.y;
	position.z = startingPosition.z;

	velocity.x = startingVelocity.x;
	velocity.y = startingVelocity.y;
	velocity.z = startingVelocity.z;

	acceleration.x = startingAcceleration.x;
	acceleration.y = startingAcceleration.y;
	acceleration.z = startingAcceleration.z;

	scale.x = startingScale.x;
	scale.y = startingScale.y;
	scale.z = startingScale.z;
	resetFlags();
}

void CompositeEntity::addEntity(Entity* toAdd){
	first->addEntity(toAdd);
	updateBounding();
}

void CompositeEntity::updateBounding(){
	if (first != nullptr){
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
				checking = checking->getNext();
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
			if (!(position.x + totalBounding.x < collidingWith->position.x - collidingWith->totalBounding.x || 
				position.x - totalBounding.x > collidingWith->position.x + collidingWith->totalBounding.x || 
				position.y + totalBounding.y < collidingWith->position.y - collidingWith->totalBounding.y || 
				position.y - totalBounding.y > collidingWith->position.y + collidingWith->totalBounding.y)){
				bool isColliding = subEntitiesColliding(first, collidingWith->first, collidingWith->first);
				if (isColliding){
					collideLeft = position.x - totalBounding.x >= collidingWith->position.x - collidingWith->totalBounding.x;
					staticCollideLeft = collideLeft && collidingWith->getEntityType() == STATIC_ENTITY;
					collideLeft = collideLeft && collidingWith->getEntityType() != STATIC_ENTITY;

					collideRight = position.x + totalBounding.x <= collidingWith->position.x + collidingWith->totalBounding.x;
					staticCollideRight = collideRight && collidingWith->getEntityType() == STATIC_ENTITY;
					collideRight = collideRight && collidingWith->getEntityType() != STATIC_ENTITY;

					collideBottom = position.y - totalBounding.y >= collidingWith->position.y - collidingWith->totalBounding.y;
					staticCollideBottom = collideBottom && collidingWith->getEntityType() == STATIC_ENTITY;
					collideBottom = collideBottom && collidingWith->getEntityType() != STATIC_ENTITY;

					collideTop = position.y + totalBounding.y <= collidingWith->position.y + collidingWith->totalBounding.y;
					staticCollideTop = collideTop && collidingWith->getEntityType() == STATIC_ENTITY;
					collideTop = collideTop && collidingWith->getEntityType() != STATIC_ENTITY;

					if (collideLeft && collideRight){
						collideRight = false;
					}
					if (collideTop && collideBottom){
						collideTop = false;
					}

					if (staticCollideLeft && staticCollideRight){
						staticCollideLeft = false;
						staticCollideRight = false;
					}
					if (staticCollideTop && staticCollideBottom){
						staticCollideTop = false;
						staticCollideBottom = false;
					}
				}
				else{
					collideLeft = false;
					collideRight = false;
					collideTop = false;
					collideBottom = false;
					staticCollideLeft = false;
					staticCollideRight = false;
					staticCollideTop = false;
					staticCollideBottom = false;
				}
				return isColliding;
			}
			return false;
		}
		return false;
	}
	return false;
}


void CompositeEntity::move(float elapsed, float gravity, float frictionX, float frictionY){
	resetFlags();
	if (type != STATIC_ENTITY){
		if (state == BOUNCING){

		}

		else if (state == BOUNCED){
			velocity.x = 0;
			velocity.y = 0;
			velocity.z = 0;
			state = STATIONARY;
		}

		else{
			velocity.x = lerp(velocity.x, 0.0f, elapsed * frictionX);
			velocity.y = lerp(velocity.y, 0.0f, elapsed * frictionY);

			velocity.x += acceleration.x * elapsed;
			velocity.y += acceleration.y * elapsed;
			velocity.z += acceleration.z * elapsed;

			velocity.y += gravity * elapsed;

			position.x += velocity.x * elapsed;
			position.y += velocity.y * elapsed;
			position.z += velocity.z * elapsed;

			rotation += rotationalVelocity * elapsed;
			if (first != nullptr){
				first->move(elapsed);
			}

			if (velocity.x != 0 || velocity.y != 0 || velocity.z != 0){
				setState(MOVING);
			}

			if (acceleration.x != 0 || acceleration.y != 0 || acceleration.z != 0){
				setState(ACCELERATING);
			}
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

//void CompositeEntity::boundaryTurnAndDown(float gameWall, float gameCeiling){
//	boundaryTurn(gameWall, gameCeiling);
//	position.y -= 0.125;
//}

//Throws exception if collision behavior unrecognized
void CompositeEntity::collide(float elapsed, CompositeEntity* bouncingOffOf){
	switch (collisionBehavior){
	case BOUNCE:
		state = BOUNCING;
		bounce(elapsed, bouncingOffOf);
		break;
	case DESTROY:
		if (!isInvincible){
			destroy();
		}
		break;
	case DEACTIVATE:
		if (!isInvincible){
			deActivate();
		}
		break;
	default:
		throw "UNRECOGNIZED COLLISION BEHAVIOR!";
		break;
	}
}

void CompositeEntity::collideWithStatic(float penetration, DIRECTION direction){
	if (collisionBehavior == DESTROY){
		destroy();
		return;
	}
	if (direction == Y){
		velocity.y = 0;
		acceleration.y = 0;
		if (staticCollideBottom){
			position.y += penetration + 0.01;
		}

		if (staticCollideTop){
			position.y -= penetration + 0.01;
		}
	}
	else if (direction == X){
		velocity.x = 0;
		acceleration.x = 0;
		if (staticCollideLeft){
			position.x += penetration + 0.01;
		}

		if (staticCollideRight){
			position.x -= penetration + 0.01;
		}
	}
}

void CompositeEntity::resetFlags(){
	boundaryLeft = false;
	boundaryRight = false;
	boundaryBottom = false;
	boundaryTop = false;

	collideLeft = false;
	collideRight = false;
	collideBottom = false;
	collideTop = false;

	staticCollideLeft = false;
	staticCollideRight = false;
	staticCollideBottom = false;
	staticCollideTop = false;
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
	//case BOUND_TURN_AND_DOWN:
	//	boundaryTurnAndDown(gameWall, gameCeiling);
	//	break;
	case BOUND_NOTHING:
		break;
	default:
		throw "UNKNOWN BOUNDARY BEHAVIOR!";
		break;
	}
}

float CompositeEntity::lerp(float v0, float v1, float t){
	return (1 - t)*v0 + t*v1;
}

void CompositeEntity::blink(){
	first->blinkAll();
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

void CompositeEntity::setDisplayText(const std::string& text){
	return;
}

void CompositeEntity::generateText(){
	return;
}

const std::string& CompositeEntity::getDisplayText(){
	return "";
}