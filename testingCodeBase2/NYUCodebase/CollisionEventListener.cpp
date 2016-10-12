#include "CollisionEventListener.h"
#include "Entity.h"

CollisionEventListener::CollisionEventListener()
{
}


CollisionEventListener::~CollisionEventListener()
{
}

void CollisionEventListener::addEvent(CollisionEvent eventToAdd){
	collisionEvents.push_back(eventToAdd);
}

void CollisionEventListener::listen(){
	for (CollisionEvent toDo : collisionEvents){
		toDo();
	}
}