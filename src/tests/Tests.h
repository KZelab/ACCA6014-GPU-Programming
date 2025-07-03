#pragma once
#include <functional>
#include <string>
#include <vector>



namespace test
{
	class Tests
	{
	public:
		Tests() {}
		virtual ~Tests() {}

		virtual void Update(float deltaTime) {}
		virtual void Render() {}
		virtual void RenderGUI() {}

	};


	class TestMenu : public Tests
	{
	public:
		TestMenu(Tests*& currentTestPointer);

		void RenderGUI() override;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			m_Tests.push_back(std::make_pair(name, []() -> Tests* { return new T(); }));
		}

		template<typename T, typename... Args>
		void RegisterTest(const std::string& name, Args&&... args)
		{
			m_Tests.push_back(std::make_pair(name,
				[args = std::make_tuple(std::forward<Args>(args)...)]() -> Tests* {
					return std::apply([](auto&&... unpackedArgs) { return new T(std::forward<decltype(unpackedArgs)>(unpackedArgs)...); }, args);
				}));
		}
	private:
		Tests*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Tests* ()>>> m_Tests;

	};


}