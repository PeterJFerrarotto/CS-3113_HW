#ifndef COLLISION_LISTENER_H
#define COLLISION_LISTENER_H
#include <unordered_map>
#include <vector>

class CompositeEntity;
class CollisionListener
{
protected:
	unsigned entityType1, entityType2;

	void collide(float elapsed, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities);
public:
	CollisionListener(unsigned entityType1, unsigned entityType2);
	~CollisionListener();

	void operator()(float elapsed, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities);
};

#endif