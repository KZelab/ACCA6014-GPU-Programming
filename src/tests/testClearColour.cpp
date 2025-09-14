#include "testClearColour.h"
#include "Renderer/Renderer.h"
#include "vendor/imgui/imgui.h"     // Dear ImGui library for GUI elements

namespace test
{
	test::testClearColour::testClearColour() : m_ClearColour{0.2f, 0.3f, 0.8f, 1.0f }
	{
	}

	test::testClearColour::~testClearColour()
	{
	}

	void test::testClearColour::Update(float deltaTime)
	{
	}

	void test::testClearColour::Render()
	{
		GlCall(glClearColor(m_ClearColour[0], m_ClearColour[1], m_ClearColour[2], m_ClearColour[3]));
		GlCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void test::testClearColour::RenderGUI()
	{
		ImGui::ColorEdit4("Pick a colour:", m_ClearColour);
	}
}