#ifndef TICK_EVENT_H
#define TICK_EVENT_H
#include <unordered_map>

class Entity;
class TickEvent
{
protected:
	void(*eventToDo)(float, std::vector<Entity*>&);
	std::vector<Entity*> tickingEntities;
public:
	TickEvent();
	~TickEvent();
	void setEventToDo(void(*eventToDo)(float, std::vector<Entity*>&));

	void setTickingEntities(const std::vector<Entity*>& entities);

	void operator()(float elapsed);
};

#endif

