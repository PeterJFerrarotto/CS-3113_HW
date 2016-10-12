#include "TickEvent.h"
#include "Entity.h"
#include <vector>
#include <map>

TickEvent::TickEvent()
{
}

TickEvent::~TickEvent()
{
}

void TickEvent::setEventToDo(void(*eventToDo)(int, float)){
	this->eventToDo = eventToDo;
}

void TickEvent::operator()(float elapsed){
	eventToDo(tickingEntities, elapsed);
}

void TickEvent::setTickingEntities(int entities){
	tickingEntities = entities;
}

const std::string& TickEvent::getEventType(){
	return eventType;
}