#include "CollisionEvent.h"
#include "Entity.h"

CollisionEvent::CollisionEvent()
{
}

CollisionEvent::CollisionEvent(std::vector<Entity*>* collisionGroup1, std::vector<Entity*>* collisionGroup2, void(*eventToDo)(Entity*, Entity*)){
	this->collisionGroup1 = collisionGroup1;
	this->collisionGroup2 = collisionGroup2;
	this->eventToDo = eventToDo;
}

void CollisionEvent::setCollisionEvent(void(*eventToDo)(Entity*, Entity*)){
	this->eventToDo = eventToDo;
}

void CollisionEvent::operator()(){
	for (Entity* entity1 : *collisionGroup1){
		if (entity1->getCanCollide() && entity1->getState() != DESTROYED){
			for (Entity* entity2 : *collisionGroup2){
				if (entity2->getCanCollide() && entity2->getState() != DESTROYED){
					float entity1PosX = entity1->getPosition().x;
					float entity1PosY = entity1->getPosition().y;
					float entity1BoundingX = entity1->getBounding().x;
					float entity1BoundingY = entity1->getBounding().y;
					int entity1BoundingType = entity1->getCollisionType();

					float entity2PosX = entity2->getPosition().x;
					float entity2PosY = entity2->getPosition().y;
					float entity2BoundingX = entity2->getBounding().x;
					float entity2BoundingY = entity2->getBounding().y;
					int entity2BoundingType = entity2->getCollisionType();

					if (entity1BoundingType == SQUARE && entity2BoundingType == SQUARE){
						if (!(entity1PosY + entity1BoundingY < entity2PosY - entity2BoundingY || entity1PosY - entity1BoundingY > entity2PosY + entity2BoundingY || entity1PosX + entity1BoundingX < entity2PosX - entity2BoundingX || entity1PosX - entity1BoundingX > entity2PosX + entity2BoundingX)){
							eventToDo(entity1, entity2);
						}
					}
				}
			}
		}
	}
}

CollisionEvent::~CollisionEvent()
{
	collisionGroup1 = {};
	collisionGroup2 = {};
	eventToDo = nullptr;
}

void CollisionEvent::setCollisionGroup1(std::vector<Entity*>* collisionGroup1){
	this->collisionGroup1 = collisionGroup1;
}

void CollisionEvent::setCollisionGroup2(std::vector<Entity*>* collisionGroup2){
	this->collisionGroup2 = collisionGroup2;
}

const std::string& CollisionEvent::getEventType(){
	return eventType;
}
