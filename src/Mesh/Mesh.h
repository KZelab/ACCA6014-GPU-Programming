#pragma once
#include <vector>
#include <memory>
#include <string>

#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../VertexBuffer.h"
#include "Vertex.h"

#include "glm/glm.hpp"

class Mesh
{
protected:
	std::vector<Vertex> m_Vertices;
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
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	virtual ~Mesh();

	/*
	 * ============================================================================
	 * THE RULE OF FIVE - Move and Copy Semantics
	 * ============================================================================
	 *
	 * In C++11 and later, if you define ANY of these 5 special member functions:
	 *   1. Destructor
	 *   2. Copy constructor
	 *   3. Copy assignment operator
	 *   4. Move constructor
	 *   5. Move assignment operator
	 *
	 * ...you should consider defining ALL of them. This is the "Rule of Five".
	 *
	 * WHY WE NEED EXPLICIT MOVE OPERATIONS HERE:
	 * ------------------------------------------
	 * We declared a virtual destructor above (~Mesh). In C++11, declaring a
	 * user-defined destructor SUPPRESSES the implicit generation of move
	 * operations (move constructor and move assignment operator).
	 *
	 * Without move operations, code like this would fail to compile:
	 *     std::unique_ptr<Mesh> mesh = GeometryFactory::CreateFullscreenQuad();
	 *
	 * Because std::unique_ptr needs to MOVE ownership of the Mesh object,
	 * and without a move assignment operator, the compiler doesn't know how.
	 *
	 * WHAT "= default" MEANS:
	 * -----------------------
	 * Writing "= default" tells the compiler: "Generate the default
	 * implementation for this function." For move operations, this means
	 * moving each member variable individually:
	 *   - std::vector and std::unique_ptr have their own move operations
	 *   - glm::vec3 is trivially copyable/movable
	 *
	 * The compiler-generated move will efficiently transfer ownership of our
	 * unique_ptr members (m_VAO, m_EBO, m_VBO) without copying the GPU resources.
	 *
	 * WHY WE DELETE COPY OPERATIONS:
	 * ------------------------------
	 * Our class contains std::unique_ptr members, which CANNOT be copied
	 * (there can only be one owner of a unique_ptr at a time).
	 *
	 * Writing "= delete" explicitly disables these operations. Without this,
	 * the compiler would implicitly delete them anyway (because unique_ptr
	 * deletes its copy operations), but being explicit:
	 *   1. Documents our intent clearly
	 *   2. Gives clearer error messages when someone tries to copy
	 *   3. Follows the Rule of Five explicitly
	 *
	 * MOVE vs COPY:
	 * -------------
	 * - COPY: Creates a duplicate. Both original and copy exist independently.
	 *         Example: Mesh a; Mesh b = a;  // Now we have TWO meshes
	 *
	 * - MOVE: Transfers ownership. The original is left in a "moved-from" state.
	 *         Example: Mesh a; Mesh b = std::move(a);  // 'a' is now empty, 'b' owns everything
	 *
	 * Moving is essential for unique_ptr because there can only be ONE owner.
	 * ============================================================================
	 */

	// Move constructor: Mesh newMesh = std::move(oldMesh);
	Mesh(Mesh&&) = default;

	// Move assignment: existingMesh = std::move(otherMesh);
	Mesh& operator=(Mesh&&) = default;

	// Copy constructor: DELETED - can't copy unique_ptr members
	Mesh(const Mesh&) = delete;

	// Copy assignment: DELETED - can't copy unique_ptr members
	Mesh& operator=(const Mesh&) = delete;

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

