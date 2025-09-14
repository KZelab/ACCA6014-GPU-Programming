#pragma once
#include "Tests.h"

namespace test
{
	class testClearColour :public Tests
	{
	public:
		testClearColour();
		~testClearColour();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;
	private:
		float m_ClearColour[4];
	};
}


