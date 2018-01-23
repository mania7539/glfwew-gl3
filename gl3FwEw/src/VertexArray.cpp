#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	unsigned int offset = 0;
	for (unsigned int i=0; i<elements.size(); i++)
	{
		const auto& element = elements[i];
		// below code for vertex array object concept: keep still at the first time, later can remove
		glEnableVertexAttribArray(i);			// index: it's the index we want to enable attribute.
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*) offset);
		// above code ends
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);
	}
	
}

void VertexArray::bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}