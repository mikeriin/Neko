#include "graphics/Buffer.h"

#include <glad/glad.h>


Buffer::Buffer(size_t size, const void* data)
{
	glCreateBuffers(1, &m_BufferId);
	glNamedBufferStorage(m_BufferId, size, data, GL_DYNAMIC_STORAGE_BIT);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &m_BufferId);
}
