#include "LogicEvent.h"


LogicEvent::LogicEvent()
{
}


LogicEvent::~LogicEvent()
{
}

void LogicEvent::setLogic(void(*logicEvent)(float, std::unordered_map<std::string, Entity*>)){
	this->logicEvent = logicEvent;
}

void LogicEvent::setEntities(std::unordered_map<std::string, Entity*> entities){
	this->logicEvalEntities = entities;
}

void LogicEvent::operator()(float elapsed){
	logicEvent(elapsed, logicEvalEntities);
}
