#pragma once
#include <vector>
#include <memory>
#include <string>

#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../VertexBuffer.h"

#include "glm/glm.hpp"
class Mesh
{
protected:
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;

	//Buffers
	std::unique_ptr<VertexArray>  m_VAO;
	std::unique_ptr<IndexBuffer>  m_EBO;
	std::unique_ptr<VertexBuffer> m_VBO;

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;

public:
	Mesh() : m_Position(0.0f), m_Rotation(0.0f), m_Scale(1.0f) {};

	virtual ~Mesh();

	virtual void SetupMesh();
	virtual void Draw();


	void setPosition(const glm::vec3& position) { m_Position = position; }
	void setRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
	void setScale(const glm::vec3& scale) { m_Scale = scale; }

	glm::vec3 getPosition() const { return m_Position; }
	glm::vec3 getRotation() const { return m_Rotation; }
	glm::vec3 getScale()    const { return m_Scale;    }

	glm::mat4 getTransformMatrix() const;


};

