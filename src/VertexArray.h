#pragma once
#include "VertexBuffer.h"

#include <iostream>
class VertexBufferLayout; //forward declare it rather than importing to save the circler dependency issue with renderer 

class VertexArray
{
private:
	unsigned int m_RendererID;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

	void Bind() const;

	void unBind() const;
};