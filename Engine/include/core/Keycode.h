#ifndef KEYCODE_H
#define KEYCODE_H


#ifdef _WIN32


const unsigned char MAX_KEY_SIZE = 255;


enum Keycode
{
	KEY_NONE = 0x00,

	KEY_Z = 0x5A,
	KEY_Q = 0x51,
	KEY_S = 0x53,
	KEY_D = 0x44,

	KEY_ESCAPE = 0x1B,
	KEY_TAB = 0x09,

	KEY_SPACE = 0x20,
	KEY_CTRL = 0x11,
	KEY_SHIFT = 0x10,

	MOUSE_LEFT = 0x01,
	MOUSE_MIDDLE = 0x02,
	MOUSE_RIGHT = 0x04,
};


#endif // _WIN32




#endif // !KEYCODE_H
