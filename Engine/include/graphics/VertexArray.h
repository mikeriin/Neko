#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H


#include "Neko.h"

#include <glad/glad.h>



class VertexArray
{
public:
	VertexArray() = delete;
	VertexArray(u32 vbo, i32 size);
	VertexArray(u32 vbo, i32 size, u32 ebo);
	~VertexArray();

	void LinkAttrib(u32 layout, i32 size, u32 type, i32 offset) const;
	void LinkAttribI(u32 layout, i32 size, u32 type, i32 offset) const;
	inline void BindElementBuffer(u32 ebo) const { glVertexArrayElementBuffer(m_VertexArrayId, ebo); }

	inline void Bind() const { glBindVertexArray(m_VertexArrayId); }


private:
	u32 m_VertexArrayId;
};


#endif // !VERTEX_ARRAY_H

