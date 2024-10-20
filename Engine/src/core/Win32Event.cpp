#include "core/Event.h"

#ifdef _WIN32


#include <Windows.h>


std::unordered_map<i32, KeyState> EventHandler::m_KeyStates = {};


EventHandler::~EventHandler()
{
	m_KeyStates.clear();
}

void EventHandler::TrackKey(Keycode code)
{
	if (m_KeyStates.find(code) == m_KeyStates.end())
	{
		m_KeyStates.emplace(code, KeyState::UP);
	}
}

void EventHandler::PollEvents()
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	for (auto& kState : m_KeyStates)
	{
		SHORT state = GetAsyncKeyState(kState.first);
		bool isCurrentlyDown = (state & 0x8000) != 0;

		if (isCurrentlyDown)
		{
			if (kState.second == KeyState::UP || kState.second == KeyState::RELEASED) kState.second = KeyState::PRESSED;
			else if (kState.second == KeyState::PRESSED) kState.second = KeyState::DOWN;
		}
		else
		{
			if (kState.second == KeyState::DOWN || kState.second == KeyState::PRESSED) kState.second = KeyState::RELEASED;
			else if (kState.second == KeyState::RELEASED) kState.second = KeyState::UP;
		}
	}

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(GetActiveWindow(), &p);

	m_MouseRelX = p.x - m_MouseX;
	m_MouseRelY = p.y - m_MouseY;

	m_MouseX = p.x;
	m_MouseY = p.y;

	if (m_IsRelative)
	{
		RECT windowRect;
		GetClientRect(GetActiveWindow(), &windowRect);
		int centerX = (windowRect.right - windowRect.left) / 2;
		int centerY = (windowRect.bottom - windowRect.top) / 2;
		SetCursorPos(centerX, centerY);  // Set cursor to center
		m_MouseX = centerX;
		m_MouseY = centerY;
	}
}

bool EventHandler::IsPressed(Keycode code) const
{
	auto it = m_KeyStates.find(code);
	return it != m_KeyStates.end() && it->second == KeyState::PRESSED;
}

bool EventHandler::IsDown(Keycode code) const
{
	auto it = m_KeyStates.find(code);
	return it != m_KeyStates.end() && (it->second == KeyState::DOWN || it->second == KeyState::PRESSED);
}

bool EventHandler::IsReleased(Keycode code) const
{
	auto it = m_KeyStates.find(code);
	return it != m_KeyStates.end() && it->second == KeyState::RELEASED;
}

bool EventHandler::IsUp(Keycode code) const
{
	auto it = m_KeyStates.find(code);
	return it != m_KeyStates.end() && (it->second == KeyState::UP || it->second == KeyState::RELEASED);
}

void EventHandler::GetRelativeMousePos(i32& dX, i32& dY) const
{
	dX = m_MouseRelX;
	dY = m_MouseRelY;
}

void EventHandler::GetMousePos(i32& mX, i32& mY) const
{
	mX = m_MouseX;
	mY = m_MouseY;
}


#endif // _WIN32
