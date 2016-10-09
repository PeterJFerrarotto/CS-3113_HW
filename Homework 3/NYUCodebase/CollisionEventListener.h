#ifndef COLLISION_EVENT_LISTENER_H
#define COLLISION_EVENT_LISTENER_H
#include <map>
#include <unordered_map>
#include "CollisionEvent.h"
class CollisionEventListener final
{
protected:
	std::vector<CollisionEvent> collisionEvents;
	
public:
	CollisionEventListener();
	~CollisionEventListener();

	void addEvent(CollisionEvent eventToAdd);
	void listen();
};

#endif