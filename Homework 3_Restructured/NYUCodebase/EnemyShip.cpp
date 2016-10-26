#include "EnemyShip.h"
#include "Ship.h"
#include "CompositeEntity.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "Entity.h"
EnemyShip::EnemyShip()
{
}

EnemyShip::EnemyShip(Entity* first, float firingDelay) : Ship(first, firingDelay){
	timeSinceFiring = firingDelay;
	updateBounding();
}

EnemyShip::~EnemyShip()
{
}

CompositeEntity* EnemyShip::logic(CompositeEntity* player, Texture* projectileTexture, CompositeEntity* lastProjectile){
	if ((player->getPosition().x - player->getTotalBounding().x <= position.x + totalBounding.x && player->getPosition().x + player->getTotalBounding().x >= position.x - totalBounding.x) && isActive){
		if (lastProjectile == nullptr){
			return fire(projectileTexture);
		}
		else{
			if (abs((abs(lastProjectile->getPosition().y) - abs(position.y))) >= 1.5){
				return fire(projectileTexture);
			}
		}
	}
	return nullptr;
}

CompositeEntity* EnemyShip::fire(Texture* projectileTexture){
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
		collectiveProjectile->setVelocity(0, -1.5);
		collectiveProjectile->setPosition(position.x, position.y - 0.2);
		collectiveProjectile->setScale(0.5, 0.3);
		collectiveProjectile->setRotation(M_PI);
		collectiveProjectile->setBoundingType(SQUARE);
		collectiveProjectile->setEntityType(ENEMY_PROJECTILE);
		collectiveProjectile->setCollisionBehavior(DESTROY);
		collectiveProjectile->setBoundaryBehavior(BOUND_DESTROY);
		collectiveProjectile->setIsActive(true);
		collectiveProjectile->setCanCollide(true);

		timeSinceFiring = 0;
		return collectiveProjectile;
	}
	return nullptr;
}