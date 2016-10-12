#ifndef TICK_EVENT_H
#define TICK_EVENT_H
#include <map>

class Entity;
class TickEvent
{
protected:
	void(*eventToDo)(int, float);
	//std::map<std::string, Entity*> tickingEntities;
	int tickingEntities;
	const std::string eventType = "Tick";
public:
	TickEvent();
	~TickEvent();
	void setEventToDo(void(*eventToDo)(int, float));

	void setTickingEntities(int entities);

	void operator()(float elapsed);

	const std::string& getEventType();
};

#endif

