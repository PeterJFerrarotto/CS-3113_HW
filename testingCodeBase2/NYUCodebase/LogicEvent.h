#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H
#include <vector>

class Entity;
class LogicEvent
{
protected:
	void(*logicEvent)(float, Entity*);
	std::vector<Entity*>* logicEvalEntities;

	const std::string& eventType = "Logic";
public:
	LogicEvent();
	~LogicEvent();

	void setLogic(void(*logicEvent)(float, Entity*));

	void setEntities(std::vector<Entity*>* entities);

	void operator()(float elapsed);

	const std::string& getEventType();
};

#endif