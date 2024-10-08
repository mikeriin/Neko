#include "core/Window.h"


#ifdef _WIN32

#include <iostream>
#include <Windows.h>
#include <glad/glad_wgl.h>


const wchar_t WND_CLASS_NAME[] = L"WND CLASS";

static LRESULT Wndproc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	Window* wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
	case WM_SIZE:
	{
		if (!wnd) break;

		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		glViewport(0, 0, width, height);
	}
		break;

	case WM_CLOSE:
		if (wnd)
		{
			wnd->Close();
		}
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return NULL;
}


static const wchar_t* ConvertCharToWChar(
	const char* charArray
) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, charArray, -1, NULL, 0);
	wchar_t* wideCharArray = new wchar_t[size_needed];
	MultiByteToWideChar(CP_UTF8, 0, charArray, -1, wideCharArray, size_needed);

	return wideCharArray;
}


Window::Window()
	: m_Handle(nullptr)
	, m_GraphicsHandle(nullptr)
	, m_GLContext(nullptr)
	, m_IsOpen(false)
{
}

Window::~Window()
{
	if (m_GLContext)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext((HGLRC)m_GLContext);
	}
		
	if (m_GraphicsHandle)
	{
		DeleteDC((HDC)m_GraphicsHandle);
	}

	if (m_Handle)
	{
		DestroyWindow((HWND)m_Handle);
	}
}


bool Window::Create(
	int width, 
	int height, 
	const char* title
) {
	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = Wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND + 1;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WND_CLASS_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	RECT wndRect{ 0, 0, width, height };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, false);
	int newWidth = wndRect.right - wndRect.left;
	int newHeight = wndRect.bottom - wndRect.top;

	if (!RegisterClassEx(&wc))
	{
		std::cerr << "WINDOW::ERROR::RegisterClassEx:" << GetLastError() << "\n";
		return false;
	}

	m_Handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		WND_CLASS_NAME,
		ConvertCharToWChar(title),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		newWidth, // width
		newHeight, // height
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!m_Handle)
	{
		std::cerr << "WINDOW::ERROR::CreateWindowEx\n";
		return false;
	}

	SetWindowLongPtr((HWND)m_Handle, GWLP_USERDATA, (LONG_PTR)this);

	m_GraphicsHandle = GetDC((HWND)m_Handle);

	PIXELFORMATDESCRIPTOR pfd{};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pixelFormat = ChoosePixelFormat((HDC)m_GraphicsHandle, &pfd);
	
	if (!SetPixelFormat((HDC)m_GraphicsHandle, pixelFormat, &pfd))
	{
		std::cerr << "WINDOW::ERROR::SetPixelFormat\n";
		return false;
	}


	HGLRC tempContext = wglCreateContext((HDC)m_GraphicsHandle);
	wglMakeCurrent((HDC)m_GraphicsHandle, tempContext);


	if (!gladLoadWGL((HDC)m_GraphicsHandle))
	{
		std::cerr << "WINDOW::ERROR::gladLoadWGL\n";
		return false;
	}

	if (!gladLoadGL())
	{
		std::cerr << "WINDOW::ERROR::gladLoadGL\n";
		return false;
	}


	wglMakeCurrent((HDC)m_GraphicsHandle, NULL);
	wglDeleteContext(tempContext);


	int piAttribList[] = {
		WGL_DRAW_TO_WINDOW_ARB, TRUE,
		WGL_SUPPORT_OPENGL_ARB, TRUE,
		WGL_DOUBLE_BUFFER_ARB, TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};

	int format;
	UINT numFormat;

	wglChoosePixelFormatARB((HDC)m_GraphicsHandle, piAttribList, nullptr, 1, &format, &numFormat);

	if (numFormat)
	{
		PIXELFORMATDESCRIPTOR pfd = {};
		DescribePixelFormat((HDC)m_GraphicsHandle, format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat((HDC)m_GraphicsHandle, format, &pfd);
	}
	else
	{
		std::cerr << "WINDOW::ERROR::wglChoosePixelFormatARB\n";
		return false;
	}


	const int glAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	m_GLContext = wglCreateContextAttribsARB((HDC)m_GraphicsHandle, 0, glAttribs);

	if (!m_GLContext)
	{
		std::cerr << "WINDOW::ERROR::wglCreateContextAttribsARB\n";
		return false;
	}

	wglMakeCurrent((HDC)m_GraphicsHandle, (HGLRC)m_GLContext);

	m_IsOpen = true;
	return true;
}


void Window::RenderSwap() const
{
	SwapBuffers((HDC)m_GraphicsHandle);
}


#endif // _WIN32
