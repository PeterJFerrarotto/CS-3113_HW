#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H
#include <unordered_map>

class Entity;
class LogicEvent
{
protected:
	void(*logicEvent)(float, std::unordered_map<std::string, Entity*> entities);
	std::unordered_map<std::string, Entity*> logicEvalEntities;
public:
	LogicEvent();
	~LogicEvent();

	void setLogic(void(*logicEvent)(float, std::unordered_map<std::string, Entity*>));

	void setEntities(std::unordered_map<std::string, Entity*> entities);

	void operator()(float elapsed);
};

#endif