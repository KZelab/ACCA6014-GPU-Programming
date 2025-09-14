#pragma once

class IndexBuffer //In OpenGL, an index buffer (also known as an element buffer or EBO) is used to store indices that reference vertices in a vertex buffer. Instead of sending redundant vertex data multiple times for shapes that share vertices, like a cube or a mesh, the index buffer holds the order in which vertices should be drawn to form triangles or other primitives. Each index in the buffer corresponds to a vertex in the vertex buffer, allowing OpenGL to reuse vertex data efficiently. When rendering, the GPU looks at the index buffer to know which vertices to connect, reducing memory usage and improving performance by avoiding duplicated vertex data. This is especially useful for complex models with many shared vertices.
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	inline unsigned int GetCount() const { return m_Count; }
};