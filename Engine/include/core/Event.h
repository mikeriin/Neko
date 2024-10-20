#ifndef EVENT_H
#define EVENT_H


#include "Neko.h"

#include <unordered_map>

#include "core/Key.h"



enum Keycode;
enum class KeyState;


class EventHandler
{
public:
	EventHandler() = default;
	~EventHandler();

	static void TrackKey(Keycode code);
	void PollEvents();

	bool IsPressed(Keycode code) const;
	bool IsDown(Keycode code) const;
	bool IsReleased(Keycode code) const;
	bool IsUp(Keycode code) const;

	void GetRelativeMousePos(i32& dX, i32& dY) const;
	void GetMousePos(i32& mX, i32& mY) const;

	void ShowCursor() {}
	void SetRelativeMouseMode(bool isRelative) { m_IsRelative = isRelative; }

private:
	static std::unordered_map<i32, KeyState> m_KeyStates;

	i32 m_MouseRelX;
	i32 m_MouseRelY;
	i32 m_MouseX;
	i32 m_MouseY;

	bool m_IsRelative;
};




#endif // !EVENT_H
