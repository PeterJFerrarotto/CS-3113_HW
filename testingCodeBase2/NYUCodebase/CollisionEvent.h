#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H
#include <string>
#include <vector>

class Entity;

class CollisionEvent
{
protected:
	void (*eventToDo)(Entity*, Entity*);
	std::vector<Entity*> *collisionGroup1;
	std::vector<Entity*> *collisionGroup2;

	const std::string eventType = "Collision";
public:
	CollisionEvent();
	CollisionEvent(std::vector<Entity*>* collisionGroup1, std::vector<Entity*>* collisionGroup2, void(*eventToDo)(Entity*, Entity*));
	~CollisionEvent();

	void setCollisionEvent(void (*eventToDo)(Entity*, Entity*));

	void operator()();

	void setCollisionGroup1(std::vector<Entity*>* collisionGroup1);
	void setCollisionGroup2(std::vector<Entity*>* collisionGroup2);

	const std::string& getEventType();

};

#endif