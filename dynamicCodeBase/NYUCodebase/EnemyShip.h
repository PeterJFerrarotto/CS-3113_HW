#ifndef ENEMY_SHIP_H
#define ENEMY_SHIP_H

#include "CompositeEntity.h"
#include <math.h>

class EnemyShip: public CompositeEntity
{
public:
	EnemyShip();
	EnemyShip(Entity* first, float firingDelay, Texture* projectileTexture = nullptr);
	~EnemyShip();

	CompositeEntity* logic(CompositeEntity* player, CompositeEntity* lastProjectile = nullptr);

	CompositeEntity* fire();
};

#endif