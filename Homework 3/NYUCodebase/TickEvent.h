#ifndef TICK_EVENT_H
#define TICK_EVENT_H
#include <map>

class Entity;
class TickEvent
{
protected:
	void(*eventToDo)(float, int);
	//std::map<std::string, Entity*> tickingEntities;
	int tickingEntities;
	const std::string eventType = "Tick";
public:
	TickEvent();
	~TickEvent();
	void setEventToDo(void(*eventToDo)(float, int));

	void setTickingEntities(int entities);

	void operator()(float elapsed);

	const std::string& getEventType();
};

#endif

