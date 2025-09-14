#include "VertexArray.h"
#include "VertexBufferLayout.h"
VertexArray::VertexArray()
{
    GlCall(glGenVertexArrays(1, &m_RendererID));
    GlCall(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
    GlCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{

    Bind();
	vb.Bind();
    const auto& Elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < Elements.size(); i++)
    {
        const auto& element = Elements[i];
        // Enable the first attribute (position) in the shader
        GlCall(glEnableVertexAttribArray(i));
        // Specify how OpenGL should interpret the vertex data
        GlCall(glVertexAttribPointer(i, element.count, element.type, 
            element.normalised, 
            layout.GetStride(), 
            (const void*)offset));

        offset += element.count * VertexBufferElement::getSizeOfType(element.type);

    }

}

void VertexArray::Bind() const
{
    GlCall(glBindVertexArray(m_RendererID));
}

void VertexArray::unBind() const
{
    GlCall(glBindVertexArray(0));
}
