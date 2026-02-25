#include "Tests.h"
#include "DefaultScene.h"
#include "../vendor/imgui/imgui.h"


namespace test
{
    void Tests::InitDefaultScene()
    {
        m_DefaultScene = std::make_unique<DefaultScene>();
    }

	TestMenu::TestMenu(Tests*& currentTestPointer) : m_CurrentTest(currentTestPointer)
	{

	}


	void TestMenu::RenderGUI()
	{
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
			{
				m_CurrentTest = test.second();
			}
		}
	}
}

