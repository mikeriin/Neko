#include "core/Event.h"

#ifdef _WIN32


#include <iostream>
#include <Windows.h>
#include <windowsx.h>


EventHandler::EventHandler()
	: m_MousePosition(std::make_pair(0, 0))
	, m_MouseRelative(std::make_pair(0, 0))
{
	for (unsigned char i = 0; i < MAX_KEY_SIZE; i++)
	{
		m_KeyPressed[i] = false;
		m_KeyReleased[i] = false;
		m_KeyJustPressed[i] = false;
	}
}

void EventHandler::PollEvents()
{
	MSG msg;

	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		switch (msg.message)
		{
		case WM_KEYDOWN:
			if (m_KeyPressed[msg.wParam] == false) m_KeyJustPressed[msg.wParam] = true;
			m_KeyPressed[msg.wParam] = true;

			break;

		case WM_KEYUP:
			m_KeyPressed[msg.wParam] = false;
			m_KeyReleased[msg.wParam] = true;

			break;



		case WM_RBUTTONDOWN:
			if (m_KeyPressed[MOUSE_RIGHT] == false) m_KeyJustPressed[MOUSE_RIGHT] = true;
			m_KeyPressed[MOUSE_RIGHT] = true;

			break;

		case WM_MBUTTONDOWN:
			if (m_KeyPressed[MOUSE_MIDDLE] == false) m_KeyJustPressed[MOUSE_MIDDLE] = true;
			m_KeyPressed[MOUSE_MIDDLE] = true;

			break;

		case WM_LBUTTONDOWN:
			if (m_KeyPressed[MOUSE_LEFT] == false) m_KeyJustPressed[MOUSE_LEFT] = true;
			m_KeyPressed[MOUSE_LEFT] = true;

			break;



		case WM_RBUTTONUP:
			m_KeyPressed[MOUSE_RIGHT] = false;
			m_KeyReleased[MOUSE_RIGHT] = true;

			break;

		case WM_MBUTTONUP:
			m_KeyPressed[MOUSE_MIDDLE] = false;
			m_KeyReleased[MOUSE_MIDDLE] = true;

			break;

		case WM_LBUTTONUP:
			m_KeyPressed[MOUSE_LEFT] = false;
			m_KeyReleased[MOUSE_LEFT] = true;

			break;








		case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(msg.lParam);
			int y = GET_Y_LPARAM(msg.lParam);

			m_MouseRelative.first = m_MousePosition.first - x;
			m_MouseRelative.second = m_MousePosition.second - y;


			m_MousePosition.first = x;
			m_MousePosition.second = y;
		}
			break;

		default:
			break;
		}
	}
}

bool EventHandler::PollEvents(Event& e)
{
	MSG msg;

	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		switch (msg.message)
		{
		case WM_QUIT:
			e.type = EventType::QUIT;
			return true;

		case WM_KEYDOWN:
			e.type = EventType::KEY_DOWN;
			e.key = (Keycode)msg.wParam;

			if (m_KeyPressed[msg.wParam] == false) m_KeyJustPressed[msg.wParam] = true;
			m_KeyPressed[msg.wParam] = true;

			return true;

		case WM_KEYUP:
			e.type = EventType::KEY_UP;
			e.key = (Keycode)msg.wParam;

			m_KeyPressed[msg.wParam] = false;
			m_KeyReleased[msg.wParam] = true;

			return true;








		case WM_RBUTTONDOWN:
			e.type = BUTTON_DOWN;
			e.key = (Keycode)MOUSE_RIGHT;

			if (m_KeyPressed[MOUSE_RIGHT] == false) m_KeyJustPressed[MOUSE_RIGHT] = true;
			m_KeyPressed[MOUSE_RIGHT] = true;

			return true;

		case WM_MBUTTONDOWN:
			e.type = BUTTON_DOWN;
			e.key = (Keycode)MOUSE_MIDDLE;

			if (m_KeyPressed[MOUSE_MIDDLE] == false) m_KeyJustPressed[MOUSE_MIDDLE] = true;
			m_KeyPressed[MOUSE_MIDDLE] = true;

			return true;

		case WM_LBUTTONDOWN:
			e.type = BUTTON_DOWN;
			e.key = (Keycode)MOUSE_LEFT;

			if (m_KeyPressed[MOUSE_LEFT] == false) m_KeyJustPressed[MOUSE_LEFT] = true;
			m_KeyPressed[MOUSE_LEFT] = true;

			return true;



		case WM_RBUTTONUP:
			e.type = BUTTON_UP;
			e.key = (Keycode)MOUSE_RIGHT;

			m_KeyPressed[MOUSE_RIGHT] = false;
			m_KeyReleased[MOUSE_RIGHT] = true;

			return true;

		case WM_MBUTTONUP:
			e.type = BUTTON_UP;
			e.key = (Keycode)MOUSE_MIDDLE;

			m_KeyPressed[MOUSE_MIDDLE] = false;
			m_KeyReleased[MOUSE_MIDDLE] = true;

			return true;

		case WM_LBUTTONUP:
			e.type = BUTTON_UP;
			e.key = (Keycode)MOUSE_LEFT;

			m_KeyPressed[MOUSE_LEFT] = false;
			m_KeyReleased[MOUSE_LEFT] = true;

			return true;








		case WM_MOUSEMOVE:
			e.type = MOUSE_MOTION;
			e.key = KEY_NONE;
			e.x = GET_X_LPARAM(msg.lParam);
			e.y = GET_Y_LPARAM(msg.lParam);

			m_MouseRelative.first = m_MousePosition.first - e.x;
			m_MouseRelative.second = m_MousePosition.second - e.y;

			m_MousePosition.first = e.x;
			m_MousePosition.second = e.y;



			return true;

		default:
			e.type = EventType::NONE;
			e.key = (Keycode)0;
			break;
		}
	}

	return false;
}

bool EventHandler::KeyPressed(Keycode key) const
{
	return m_KeyPressed[key];
}

bool EventHandler::KeyJustPressed(Keycode key)
{
	bool result = m_KeyJustPressed[key];
	m_KeyJustPressed[key] = false;
	return result;
}

bool EventHandler::KeyReleased(Keycode key)
{
	bool result = m_KeyReleased[key];
	m_KeyReleased[key] = false;
	return result;
}

std::pair<i32, i32> EventHandler::GetMousePosition() const
{
	return m_MousePosition;
}

std::pair<i32, i32> EventHandler::GetMouseRelative()
{
	std::pair<i32, i32> result = m_MouseRelative;
	m_MouseRelative = std::make_pair(0, 0);
	return result;
}


#endif // _WIN32
