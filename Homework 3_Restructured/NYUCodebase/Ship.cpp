#include "Ship.h"
#include "Entity.h"
#include "Texture.h"

Ship::Ship()
{
}

Ship::Ship(Entity* first){
	this->first = first;
	updateBounding();
}

Ship::Ship(Entity* first, float firingDelay){
	this->first = first;
	this->firingDelay = firingDelay;
	timeSinceFiring = firingDelay;
	updateBounding();
}


Ship::~Ship()
{
}


CompositeEntity* Ship::fire(Texture* projectileTexture){
	if (timeSinceFiring >= firingDelay && isActive){
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

		timeSinceFiring = 0;
		return collectiveProjectile;
	}
	return nullptr;
}

void Ship::addToTimeSinceFiring(float elapsed){
	timeSinceFiring += elapsed;
}