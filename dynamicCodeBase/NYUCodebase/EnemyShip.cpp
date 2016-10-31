#include "EnemyShip.h"
#include "CompositeEntity.h"
#include <algorithm>
#include "Entity.h"
EnemyShip::EnemyShip()
{
}

EnemyShip::EnemyShip(Entity* first, float firingDelay, Texture* projectileTexture) : CompositeEntity(first, firingDelay){
	timeSinceFiring = firingDelay;
	updateBounding();
	this->projectileTexture = projectileTexture;
}

EnemyShip::~EnemyShip()
{
}

CompositeEntity* EnemyShip::logic(CompositeEntity* player, CompositeEntity* lastProjectile){
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
}

CompositeEntity* EnemyShip::fire(){
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
		collectiveProjectile->setLayer(4);

		timeSinceFiring = 0;
		return collectiveProjectile;
	}
	return nullptr;
}