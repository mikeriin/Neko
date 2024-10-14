#include "graphics/VertexArray.h"



VertexArray::VertexArray(u32 vbo, i32 size)
{
	glCreateVertexArrays(1, &m_VertexArrayId);
	glVertexArrayVertexBuffer(m_VertexArrayId, 0, vbo, 0, size);
}

VertexArray::VertexArray(u32 vbo, i32 size, u32 ebo)
	: VertexArray(vbo, size)
{
	glVertexArrayElementBuffer(m_VertexArrayId, ebo);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_VertexArrayId);
}

void VertexArray::LinkAttrib(u32 layout, i32 size, u32 type, i32 offset) const
{
	glEnableVertexArrayAttrib(m_VertexArrayId, layout); // index 0
	glVertexArrayAttribFormat(m_VertexArrayId, layout, size, type, GL_FALSE, offset); // index 0
	glVertexArrayAttribBinding(m_VertexArrayId, layout, 0); // index 0 binding 0
}

void VertexArray::LinkAttribI(u32 layout, i32 size, u32 type, i32 offset) const
{
	glEnableVertexArrayAttrib(m_VertexArrayId, layout); // index 0
	glVertexArrayAttribIFormat(m_VertexArrayId, layout, size, type, offset); // index 0
	glVertexArrayAttribBinding(m_VertexArrayId, layout, 0); // index 0 binding 0
}
