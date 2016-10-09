#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H
#include <vector>

class Entity;
template <class Event>
class EventListener
{
protected:
	std::vector<Event> events;
public:
	EventListener();
	~EventListener();

	virtual void addEvent(const Event& eventToAdd);
	virtual void listen(float elapsed);
};
#endif

