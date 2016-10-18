#ifndef SHIP_H
#define SHIP_H

#ifdef _WINDOWS
#include<GL/glew.h>
#endif

#include "CompositeEntity.h"

class Entity;
class Texture;
class Ship :
	public CompositeEntity
{
protected:
	float firingDelay;
	float topSpeed;
	float timeSinceFiring;
	GLuint projectileType;

	unsigned health;
public:
	Ship();
	Ship(Entity* first);
	Ship(Entity* first, float firingDelay);
	~Ship();

	virtual CompositeEntity* fire(Texture* projectileTexture);

	void addToTimeSinceFiring(float elapsed);
};

#endif