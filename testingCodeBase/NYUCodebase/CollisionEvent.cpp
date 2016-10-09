#include "CollisionEvent.h"
#include "Entity.h"

CollisionEvent::CollisionEvent()
{
}

CollisionEvent::CollisionEvent(Entity* entity1, Entity* entity2, void(*eventToDo)(float, Entity*, Entity*)){
	collisionEntity1 = entity1;
	collisionEntity2 = entity2;
	this->eventToDo = eventToDo;
}

void CollisionEvent::setCollisionEvent(void(*eventToDo)(float, Entity*, Entity*)){
	this->eventToDo = eventToDo;
}

void CollisionEvent::operator()(float elapsed){
	float entity1PosX = collisionEntity1->getPosition().x;
	float entity1PosY = collisionEntity1->getPosition().y;
	float entity1BoundingX = collisionEntity1->getBounding().x;
	float entity1BoundingY = collisionEntity1->getBounding().y;
	int entity1BoundingType = collisionEntity1->getCollisionType();

	float entity2PosX = collisionEntity2->getPosition().x;
	float entity2PosY = collisionEntity2->getPosition().y;
	float entity2BoundingX = collisionEntity2->getBounding().x;
	float entity2BoundingY = collisionEntity2->getBounding().y;
	int entity2BoundingType = collisionEntity2->getCollisionType();

	if (entity1BoundingType == SQUARE && entity2BoundingType == SQUARE){
		if (!(entity1PosY + entity1BoundingY < entity2PosY - entity2BoundingY || entity1PosY - entity1BoundingY > entity2PosY + entity2BoundingY || entity1PosX + entity1BoundingX < entity2PosX - entity2BoundingX || entity1PosX - entity1BoundingX > entity2PosX + entity2BoundingX)){
			eventToDo(elapsed, collisionEntity1, collisionEntity2);
		}
	}
}

CollisionEvent::~CollisionEvent()
{
	collisionEntity1 = nullptr;
	collisionEntity2 = nullptr;
	eventToDo = nullptr;
}

void CollisionEvent::setEntity1(Entity* entity1){
	collisionEntity1 = entity1;
}

void CollisionEvent::setEntity2(Entity* entity2){
	collisionEntity2 = entity2;
}