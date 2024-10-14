#ifndef BUFFER_H
#define BUFFER_H


#include "Neko.h"


class Buffer
{
public:
	Buffer() = delete;
	Buffer(size_t size, const void* data);
	~Buffer();

	const u32 GetId() const { return m_BufferId; }


private:
	u32 m_BufferId;
};


#endif // !BUFFER_H

