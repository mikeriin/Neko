#include "core/Window.h"


#ifdef _WIN32

#include <iostream>
#include <glad/glad_wgl.h>
#include <Windows.h>



extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}



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


static void GLAPIENTRY MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* msg,
	const void* data
) {
	std::string _source;
	std::string _type;
	std::string _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "UNKNOWN";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return;
		/*_severity = "NOTIFICATION";
		break;*/

	default:
		_severity = "UNKNOWN";
		break;
	}
	std::cout << "OPENGL::ERROR::" << id << ": " << _type
		<< " - " << _source
		<< " - " << _severity
		<< " - " << msg << "\n";
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

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

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


	int attribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		0, // End
	};


	int format;
	UINT numFormat;

	wglChoosePixelFormatARB((HDC)m_GraphicsHandle, attribList, NULL, 1, &format, &numFormat);

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
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
		0
	};

	m_GLContext = wglCreateContextAttribsARB((HDC)m_GraphicsHandle, 0, glAttribs);

	if (!m_GLContext)
	{
		std::cerr << "WINDOW::ERROR::wglCreateContextAttribsARB\n";
		return false;
	}

	wglMakeCurrent((HDC)m_GraphicsHandle, (HGLRC)m_GLContext);

	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "OpenGL Version: " << version << std::endl;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	m_IsOpen = true;
	return true;
}


void Window::RenderSwap() const
{
	SwapBuffers((HDC)m_GraphicsHandle);
}


#endif // _WIN32
