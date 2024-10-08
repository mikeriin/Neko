#ifndef WINDOW_H
#define WINDOW_H



class Window
{
public:
	Window();
	~Window();

	bool Create(int width, int height, const char* title);
	void RenderSwap() const;

	inline void Close() { m_IsOpen = false; }
	inline bool IsOpen() const { return m_IsOpen; }

private:
	void* m_Handle;
	void* m_GraphicsHandle;
	void* m_GLContext;

	bool m_IsOpen;
};



#endif // !WINDOW_H
