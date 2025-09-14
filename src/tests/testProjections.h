#pragma once
#include "Tests.h"

#include "../buffers/VertexBuffer.h"
#include "../buffers/VertexBufferLayout.h"
#include "../buffers/IndexBuffer.h"
#include "../buffers/VertexArray.h"
#include "../buffers/Shader.h"

#include <memory>

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

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<VertexBuffer> m_VBO;


	};




}


