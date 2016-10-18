#ifndef ENEMY_SHIP_H
#define ENEMY_SHIP_H

#include "Ship.h"
class EnemyShip :
	public Ship
{
public:
	EnemyShip();
	EnemyShip(Entity* first, float firingDelay);
	~EnemyShip();

	CompositeEntity* logic(CompositeEntity* player, Texture* projectileTexture, CompositeEntity* lastProjectile = nullptr);

	CompositeEntity* fire(Texture* projectileTexture);
};

#endif