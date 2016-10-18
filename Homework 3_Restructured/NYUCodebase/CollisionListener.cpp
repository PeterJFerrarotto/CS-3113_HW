#include "CollisionListener.h"
#include "CompositeEntity.h"
using namespace std;

CollisionListener::CollisionListener(unsigned entityType1, unsigned entityType2){
	this->entityType1 = entityType1;
	this->entityType2 = entityType2;
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
					if (tmp1->getCollisionBehavior() == BOUNCE){
						tmp1->setState(BOUNCING);
						tmp1->collide(elapsed, tmp2);
					}
					if (tmp2->getCollisionBehavior() == BOUNCE){
						tmp2->setState(BOUNCING);
						tmp2->collide(elapsed, tmp1);
					}
				}
			}
		}
	}
	else{
		for (CompositeEntity* tmp1 : gameEntities[entityType1]){
			for (CompositeEntity* tmp2 : gameEntities[entityType2]){
				if (tmp1->isColliding(tmp2)){
					tmp1->collide(elapsed, tmp2);
					tmp2->collide(elapsed, tmp1);
				}
			}
		}
	}
}

void CollisionListener::operator()(float elapsed, unordered_map<unsigned, vector<CompositeEntity*>>& gameEntities){
	collide(elapsed, gameEntities);
}