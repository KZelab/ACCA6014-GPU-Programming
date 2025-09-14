#pragma once
//a vertex buffer (VBO) is used to store vertex data, such as position, colour, normals, or texture coordinates, on the GPU for rendering. When you create and bind a VBO, you upload your vertex data from the CPU to the GPU's memory. This data is then used by the GPU to efficiently render objects in the scene. The VBO is part of the graphics pipeline, and it works in conjunction with vertex shaders to process each vertex. By storing vertex data in the GPU's memory, OpenGL minimizes the overhead of repeatedly sending data from the CPU, improving rendering performance, especially in complex scenes or real-time applications like games.

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};