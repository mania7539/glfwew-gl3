#include "IndexBuffer.h"
#include "Renderer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_Count(count)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &m_RendererID));					
	// above: Generate/Create a GL Buffer, we should provide an Integer as a memory which we can write into 

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));// How do I want to use the GL Buffer? Define it to a specific buffer:
														//	target - GL_ARRAY_BUFFER: it's just an array
														//	buffer - an integer comes from memory
														// if we use glBindBuffer(GL_ARRAY_BUFFER, 0); then GPU won't draw the triangle out since we bind something else

	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
	// above: Set data which we want to use to the specific GPU buffer
	// : STATIC, DYNAMIC: we should let GPU knows that if the buffer can be modified more than ONCE.
	// : DRAW: we want to draw things with the buffer, so use it
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));// How do I want to use the GL Buffer? Define it to a specific buffer:
														//	target - GL_ARRAY_BUFFER: it's just an array
														//	buffer - an integer comes from memory
														// if we use glBindBuffer(GL_ARRAY_BUFFER, 0); then GPU won't draw the triangle out since we bind something else

}

void IndexBuffer::unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));		// How do I want to use the GL Buffer? Define it to a specific buffer:
													//	target - GL_ARRAY_BUFFER: it's just an array
													//	buffer - an integer comes from memory
													// if we use glBindBuffer(GL_ARRAY_BUFFER, 0); then GPU won't draw the triangle out since we bind something else

}