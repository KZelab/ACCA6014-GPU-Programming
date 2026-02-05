#pragma once
#include "Tests.h"

#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"

#include <memory>
#include "../Mesh/GeometryFactory.h"

namespace test
{
	class TestProjections : public Tests
	{
    public:
        TestProjections();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	private:
        bool useOrthographic;

        std::unique_ptr<Shader> m_Shader;


		std::unique_ptr<Mesh> m_Cube;
	};




}


