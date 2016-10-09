#include "TickEvent.h"
#include "Entity.h"


TickEvent::TickEvent()
{
}


TickEvent::~TickEvent()
{
}

void TickEvent::setEventToDo(void(*eventToDo)(float, std::vector<Entity*>&)){
	this->eventToDo = eventToDo;
}

void TickEvent::operator()(float elapsed){
	eventToDo(elapsed, tickingEntities);
}

void TickEvent::setTickingEntities(const std::vector<Entity*>& entities){
	tickingEntities = entities;
}