#include "CollisionListener.h"
#include "CompositeEntity.h"
using namespace std;

CollisionListener::CollisionListener(unsigned entityType1, unsigned entityType2, COLLISION_BEHAVIOR behaviorOfEntity1, COLLISION_BEHAVIOR behaviorOfEntity2, bool entity2Down, bool entity2Up, bool entity2Left, bool entity2Right){
	this->entityType1 = entityType1;
	this->entityType2 = entityType2;
	this->behaviorOfEntity1 = behaviorOfEntity1;
	this->behaviorOfEntity2 = behaviorOfEntity2;
	//this->positionOfEntity2 = positionOfEntity2;
	down = entity2Down;
	up = entity2Up;
	left = entity2Left;
	right = entity2Right;
}

CollisionListener::~CollisionListener()
{
}

void CollisionListener::collide(float elapsed, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities){
	if (entityType1 == entityType2){
		for (size_t i = 0; i < gameEntities[entityType1].size(); i++){
			CompositeEntity* tmp1 = gameEntities[entityType1][i];
			for (size_t j = i + 1; j < gameEntities[entityType1].size(); j++){
				CompositeEntity* tmp2 = gameEntities[entityType1][j];
				if (tmp1->isColliding(tmp2)){
					bool performCollision = false;
					if (up){
						performCollision = performCollision || tmp1->getCollUpFlag();
					}

					if (down){
						performCollision = performCollision || tmp1->getCollDownFlag();
					}

					if (left){
						performCollision = performCollision || tmp1->getCollLeftFlag();
					}

					if (right){
						performCollision = performCollision || tmp1->getCollRightFlag();
					}
					if (performCollision){
						tmp1->collide(elapsed, tmp2, behaviorOfEntity1);
						tmp2->collide(elapsed, tmp1, behaviorOfEntity2);
					}
				}
			}
		}
	}
	else{
		for (CompositeEntity* tmp1 : gameEntities[entityType1]){
			for (CompositeEntity* tmp2 : gameEntities[entityType2]){
				if (tmp1->isColliding(tmp2)){
					bool performCollision = false;
					if (up){
						performCollision = performCollision || (tmp1->getCollUpFlag() && !tmp1->getCollDownFlag());
					}

					if (down){
						performCollision = performCollision || (tmp1->getCollDownFlag() && !tmp1->getCollUpFlag());
					}

					if (left){
						performCollision = performCollision || (tmp1->getCollLeftFlag() && !tmp1->getCollRightFlag());
					}

					if (right){
						performCollision = performCollision || (tmp1->getCollRightFlag() && !tmp1->getCollLeftFlag());
					}
					if (performCollision){
						tmp1->collide(elapsed, tmp2, behaviorOfEntity1);
						tmp2->collide(elapsed, tmp1, behaviorOfEntity2);
					}
				}
			}
		}
	}
}

void CollisionListener::operator()(float elapsed, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities){
	collide(elapsed, gameEntities);
}