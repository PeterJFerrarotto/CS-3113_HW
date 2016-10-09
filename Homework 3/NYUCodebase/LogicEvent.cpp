#include "LogicEvent.h"


LogicEvent::LogicEvent()
{
}


LogicEvent::~LogicEvent()
{
}

void LogicEvent::setLogic(void(*logicEvent)(float, Entity*)){
	this->logicEvent = logicEvent;
}

void LogicEvent::setEntities(std::vector<Entity*>* entities){
	this->logicEvalEntities = entities;
}

void LogicEvent::operator()(float elapsed){
	for (Entity* entity : *logicEvalEntities){
		logicEvent(elapsed, entity);
	}
}

const std::string& LogicEvent::getEventType(){
	return eventType;
}