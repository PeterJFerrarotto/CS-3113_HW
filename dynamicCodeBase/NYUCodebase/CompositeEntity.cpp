#include "CompositeEntity.h"
#include "Entity.h"

CompositeEntity::CompositeEntity()
{
}

CompositeEntity::CompositeEntity(Entity* first){
	this->first = first;
	updateBounding();
}

CompositeEntity::CompositeEntity(Entity* first, float firingDelay){
	this->first = first;
	this->firingDelay = firingDelay;
	timeSinceFiring = firingDelay;
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

bool CompositeEntity::getOnGround(){
	return staticCollideBottom;
}

bool CompositeEntity::getFalls(){
	return falls;
}

const std::string& CompositeEntity::getEntityID(){
	return entityID;
}

unsigned CompositeEntity::getLayer(){
	return layer;
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

void CompositeEntity::setFiringDelay(float firingDelay){
	this->firingDelay = firingDelay;
}

void CompositeEntity::setRotation(float rotation){
	this->rotation = rotation;
}

void CompositeEntity::setStartingRotation(float startingRotation){
	this->startingRotation = startingRotation;
}

void CompositeEntity::setRotationalVelocity(float rotationalVelocity){
	this->rotationalVelocity = rotationalVelocity;
}

void CompositeEntity::setStartingRotationalVelocity(float startingRotationalVelocity){
	this->startingRotationalVelocity = startingRotationalVelocity;
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

void CompositeEntity::setFalls(bool falls){
	this->falls = falls;
}

void CompositeEntity::setEntityID(const std::string& entityID){
	this->entityID = entityID;
}

void CompositeEntity::setisInvincible(bool isInvincible){
	this->isInvincible = isInvincible;
}

void CompositeEntity::setProjectileTexture(Texture* projectileTexture){
	this->projectileTexture = projectileTexture;
}

void CompositeEntity::setSpacing(float spacing){
	this->spacing = spacing;
}

void CompositeEntity::setSize(float size){
	this->size = size;
}

void CompositeEntity::setLayer(unsigned layer){
	this->layer = layer;
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

	rotation = startingRotation;

	rotationalVelocity = startingRotationalVelocity;
	resetFlags();
}

void CompositeEntity::updateBounding(){
	if (first != nullptr){
		Entity* checking = first;
		if (boundingType == SQUARE){
			checking->updateBounding(scale.x, scale.y, scale.z);
			totalBounding.x = (abs(first->getPosition().x) + first->getBounding().x);
			totalBounding.y = (abs(first->getPosition().y) + first->getBounding().y);
			totalBounding.z = (abs(first->getPosition().z) + first->getBounding().z);
		}
		if (checking->getSibling() != nullptr){
			updateBoundingRecurse(checking->getSibling());
		}
		if (checking->getChild() != nullptr){
			updateBoundingRecurse(checking->getChild());
		}
	}
	totalBounding.x;
	totalBounding.y;
	totalBounding.z;
}

void CompositeEntity::updateBoundingRecurse(Entity* check){
	float boundX, boundY, boundZ;
	if (check != nullptr){
		check->updateBounding(scale.x, scale.y, scale.z);
		boundX = (abs(check->getPosition().x) + first->getBounding().x);
		boundY = (abs(check->getPosition().y + first->getBounding().y));
		boundZ = (abs(check->getPosition().z + first->getBounding().z));
		if (boundX > totalBounding.x){
			totalBounding.x = boundX;
		}
		if (boundY > totalBounding.y){
			totalBounding.y = boundY;
		}
		if (boundZ > totalBounding.z){
			totalBounding.z = boundZ;
		}
		updateBoundingRecurse(check->getSibling());
		updateBoundingRecurse(check->getChild());
	}
}

bool CompositeEntity::subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat, CompositeEntity* that){
	if (firstOfThis->getBoundingType() == SQUARE && firstOfThat->getBoundingType() == SQUARE){
		if (firstOfThis->getCanCollide() && firstOfThat->getCanCollide()){
			if (!(position.x + firstOfThis->getPosition().x + firstOfThis->getBounding().x < that->position.x + firstOfThat->getPosition().x - firstOfThat->getBounding().x || position.x + firstOfThis->getPosition().x - totalBounding.x > that->position.x + firstOfThat->getPosition().x + firstOfThat->getBounding().x || position.y + firstOfThis->getPosition().y + firstOfThis->getBounding().y < that->position.y + firstOfThat->getPosition().y - firstOfThat->getBounding().y || position.y + firstOfThis->getPosition().y - firstOfThis->getBounding().y > that->position.y + firstOfThat->getPosition().y + firstOfThat->getBounding().y)){
				return true;
			}
		}
		if (firstOfThis->getSibling() != nullptr){
			if (subEntitiesColliding(firstOfThis->getSibling(), originalOfThat, originalOfThat, that)){
				return true;
			}
		}
		if (firstOfThis->getChild() != nullptr){
			if (subEntitiesColliding(firstOfThis->getChild(), originalOfThat, originalOfThat, that)){
				return true;
			}
		}

		if (firstOfThat->getSibling() != nullptr){
			if (subEntitiesColliding(firstOfThis, firstOfThat->getSibling(), originalOfThat, that)){
				return true;
			}
		}
		if (firstOfThat->getChild() != nullptr){
			if (subEntitiesColliding(firstOfThis, firstOfThat->getChild(), originalOfThat, that)){
				return true;
			}
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
				bool isColliding = subEntitiesColliding(first, collidingWith->first, collidingWith->first, collidingWith);
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

			if (falls){
				velocity.y += gravity * elapsed;
			}

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
	transformMatrix();
	first->draw(program, modelMatrix);
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
		if (!(staticCollideTop && velocity.y <= 0 && acceleration.y <= 0)){
			velocity.y = 0;
			acceleration.y = 0;
		}

		if (staticCollideBottom){
			position.y += penetration + 0.001;
		}

		if (staticCollideTop){
			position.y -= penetration + 0.001;
		}
	}
	else if (direction == X){
		if (staticCollideLeft && (!staticCollideBottom || staticCollideTop)){
			velocity.x = 0;
			acceleration.x = 0;
			position.x += penetration + 0.01;
		}

		if (staticCollideRight && (!staticCollideBottom || staticCollideTop)){
			velocity.x = 0;
			acceleration.x = 0;
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

void CompositeEntity::transformMatrix(){
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Rotate(rotation);
	modelMatrix.Scale(scale.x, scale.y, scale.z);
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

CompositeEntity* CompositeEntity::fire(){
	if (timeSinceFiring >= firingDelay && isActive && projectileTexture != nullptr){
		Entity* projectile = new Entity("projectile", projectileTexture);

		projectile->setPosition(0, 0, 0);
		projectile->setVelocity(0, 0, 0);
		projectile->setAcceleration(0, 0, 0);
		projectile->setScale(1.5, 1, 1);
		projectile->setBoundingType(SQUARE);
		projectile->setRotation(0);
		projectile->setCanCollide(true);
		projectile->setDoRender(true);

		CompositeEntity* collectiveProjectile = new CompositeEntity(projectile);
		collectiveProjectile->setVelocity(0, 1.5);
		collectiveProjectile->setPosition(position.x, position.y + 0.2);
		collectiveProjectile->setScale(0.5, 0.3);
		collectiveProjectile->setRotation(0);
		collectiveProjectile->setBoundingType(SQUARE);
		collectiveProjectile->setEntityType(PLAYER_PROJECTILE);
		collectiveProjectile->setCollisionBehavior(DESTROY);
		collectiveProjectile->setBoundaryBehavior(BOUND_DESTROY);
		collectiveProjectile->setIsActive(true);
		collectiveProjectile->setCanCollide(true);
		collectiveProjectile->setLayer(4);

		timeSinceFiring = 0;
		return collectiveProjectile;
	}
	return nullptr;
}

void CompositeEntity::addToTimeSinceFiring(float elapsed){
	timeSinceFiring += elapsed;
}

CompositeEntity* CompositeEntity::logic(CompositeEntity* player, CompositeEntity* lastProjectile){
	switch (type){
	case ACTOR_ENEMY:
		if ((player->getPosition().x - player->getTotalBounding().x <= position.x + totalBounding.x && player->getPosition().x + player->getTotalBounding().x >= position.x - totalBounding.x) && isActive){
			if (lastProjectile == nullptr){
				return fire();
			}
			else{
				if (abs((abs(lastProjectile->getPosition().y) - abs(position.y))) >= 1.5){
					return fire();
				}
			}
		}
		return nullptr;
		break;
	default:
		return nullptr;
		break;
	}
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

void CompositeEntity::setTextSheet(GLuint textSheet){
	return;
}