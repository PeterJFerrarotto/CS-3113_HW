#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H

class Entity;

class CollisionEvent
{
protected:
	void (*eventToDo)(float, Entity*, Entity*);
	Entity* collisionEntity1;
	Entity* collisionEntity2;
public:
	CollisionEvent();
	CollisionEvent(Entity* entity1, Entity* entity2, void(*eventToDo)(float, Entity*, Entity*));
	~CollisionEvent();

	void setCollisionEvent(void (*eventToDo)(float, Entity*, Entity*));

	void performEvent(float elapsed);
	void operator()(float elapsed);
	void setEntity1(Entity* entity1);
	void setEntity2(Entity* entity2);
};

#endif