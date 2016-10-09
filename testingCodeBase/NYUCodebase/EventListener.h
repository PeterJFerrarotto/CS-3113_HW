#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H
#include <vector>

template <class Event>
class EventListener final
{
protected:
	std::vector<Event> events;
public:
	EventListener();
	~EventListener();

	void addEvent(const Event& eventToAdd);
	void listen(float elapsed);
};
#endif

