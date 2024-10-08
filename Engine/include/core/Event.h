#ifndef EVENT_H
#define EVENT_H


#include <cinttypes>
#include <memory>
#include <utility>

#include "Neko.h"

#include "Keycode.h"


enum EventType
{
	NONE,
	QUIT,
	KEY_DOWN,
	KEY_UP,
	MOUSE_MOTION,
	BUTTON_DOWN,
	BUTTON_UP,
};


struct Event
{
	EventType type;
	Keycode key;
	int x;
	int y;
};


class EventHandler
{
public:
	EventHandler();
	~EventHandler() = default;

	void PollEvents();
	bool PollEvents(Event& e);

	bool KeyPressed(Keycode key) const;
	bool KeyJustPressed(Keycode key);
	bool KeyReleased(Keycode key);

	std::pair<i32, i32> GetMousePosition() const;
	std::pair<i32, i32> GetMouseRelative();

private:
	bool m_KeyPressed[MAX_KEY_SIZE];
	bool m_KeyJustPressed[MAX_KEY_SIZE];
	bool m_KeyReleased[MAX_KEY_SIZE];
	
	std::pair<i32, i32> m_MousePosition;
	std::pair<i32, i32> m_MouseRelative;
};



#endif // !EVENT_H
