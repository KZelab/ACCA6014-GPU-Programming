#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    GlCall(glGenBuffers(1, &m_RendererID));
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));// Bind the buffer as an array buffer to upload vertex data
    GlCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));// Upload the vertex data to the GPU buffer
}

VertexBuffer::VertexBuffer(unsigned int size)
{
    GlCall(glGenBuffers(1, &m_RendererID));
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GlCall(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
}

void VertexBuffer::Update(const void* data, unsigned int size, unsigned int offset) const
{
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GlCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

VertexBuffer::~VertexBuffer()
{
    GlCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); 
}

void VertexBuffer::Unbind() const
{
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
