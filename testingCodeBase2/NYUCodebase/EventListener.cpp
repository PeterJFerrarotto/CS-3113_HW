#include "EventListener.h"
#include "Entity.h"
#include "CollisionEvent.h"
#include "TickEvent.h"
#include "LogicEvent.h"

template class EventListener<TickEvent>;
template class EventListener<LogicEvent>;

template <class Event>
EventListener<Event>::EventListener()
{
}

template <class Event>
EventListener<Event>::~EventListener()
{
}

template <class Event>
void EventListener<Event>::listen(float elapsed){
	for (size_t i = 0; i < events.size(); i++){
		(events[i])(elapsed);
	}
}

template <class Event>
void EventListener<Event>::addEvent(const Event& eventToAdd){
	events.push_back(eventToAdd);
}
