/**
 * Tests.h
 *
 * A lightweight test framework implementing the FACTORY PATTERN with
 * advanced C++ template metaprogramming for dynamic test registration.
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. Polymorphism and Virtual Functions
 * 2. Factory Pattern with Lambda Functions
 * 3. Variadic Templates (parameter packs)
 * 4. Perfect Forwarding (std::forward)
 * 5. std::function and Type Erasure
 * 6. std::apply and Tuple Unpacking
 * 7. Reference-to-Pointer for External State Modification
 *
 * ARCHITECTURE OVERVIEW:
 * ----------------------
 *
 *   TestMenu                         Tests (base)
 *   +------------------+             +------------------+
 *   | m_Tests[]        |             | virtual Update() |
 *   |  - name          |             | virtual Render() |
 *   |  - factory()  ---|------------>| virtual RenderGUI|
 *   | m_CurrentTest& --|--+          +------------------+
 *   +------------------+  |                   ^
 *                         |                   |
 *                         v          +--------+--------+
 *                    Tests* (active) |        |        |
 *                                 TestA   TestB   TestC
 *
 * The TestMenu holds a list of (name, factory) pairs. When user selects
 * a test, the factory lambda is called to instantiate the concrete test class.
 */

#pragma once
#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "DefaultScene.h"

namespace test
{
    // =========================================================================
    // BASE TEST CLASS
    // =========================================================================
    /**
     * Tests - Abstract base class for all test cases
     *
     * POLYMORPHISM:
     * This is the base class that defines the interface all tests must implement.
     * Using virtual functions allows us to store different test types in the same
     * container and call the correct implementation at runtime.
     *
     * VIRTUAL DESTRUCTOR:
     * Critical for polymorphic base classes! Without it, deleting a derived
     * class through a base pointer would only call the base destructor,
     * causing memory leaks and undefined behavior.
     *
     *   Tests* test = new DerivedTest();
     *   delete test;  // Without virtual ~Tests(), DerivedTest destructor won't run!
     *
     * DEFAULT IMPLEMENTATIONS:
     * Empty {} bodies provide default "do nothing" behavior, so derived classes
     * only need to override methods they actually use.
     */
    class Tests
    {
    public:
        Tests() {}
        virtual ~Tests() {}  // MUST be virtual for proper cleanup of derived classes

        virtual void Update(float deltaTime) {}  // Called each frame for logic
        virtual void Render() {}                 // Called each frame for drawing
        virtual void RenderGUI() {}              // Called for ImGui interface

    protected:
        // Null unless InitDefaultScene() was called in the child's constructor.
        std::unique_ptr<DefaultScene> m_DefaultScene;

        // Call once in a child test's constructor to activate the default scene.
        void InitDefaultScene();
    };


    // =========================================================================
    // TEST MENU (Factory Pattern Implementation)
    // =========================================================================
    /**
     * TestMenu - Manages test registration and instantiation
     *
     * FACTORY PATTERN:
     * Instead of hardcoding test creation, we store "factory functions" that
     * know how to create each test type. This decouples the menu from specific
     * test implementations.
     *
     * Without factories (bad - tight coupling):
     *   if (name == "TestA") return new TestA();
     *   if (name == "TestB") return new TestB(arg1, arg2);
     *   // Must modify this code for every new test!
     *
     * With factories (good - open for extension):
     *   menu.RegisterTest<TestA>("TestA");
     *   menu.RegisterTest<TestB>("TestB", arg1, arg2);
     *   // Adding tests doesn't require modifying TestMenu!
     *
     * TYPE ERASURE WITH std::function:
     * std::function<Tests*()> can hold ANY callable that returns Tests*.
     * This "erases" the concrete type of the lambda, allowing different
     * factory lambdas to be stored in the same vector.
     */
    class TestMenu : public Tests
    {
    public:
        /**
         * Constructor takes a REFERENCE TO A POINTER
         *
         * REFERENCE-TO-POINTER (Tests*&):
         * This allows TestMenu to modify a pointer that lives OUTSIDE the class.
         *
         *   Tests* currentTest = nullptr;
         *   TestMenu menu(currentTest);  // menu holds reference to currentTest
         *   // When user selects a test, menu can set currentTest = new SelectedTest()
         *   // The caller's currentTest pointer is updated!
         *
         * Why not just Tests**?
         * - References can't be null (safer)
         * - Cleaner syntax: m_CurrentTest = x; vs *m_CurrentTest = x;
         * - Reference semantics match the intent better
         */
        TestMenu(Tests*& currentTestPointer);

        void RenderGUI() override;

        // =====================================================================
        // SIMPLE REGISTRATION (No Constructor Arguments)
        // =====================================================================
        /**
         * RegisterTest<T>(name) - Register a test that needs no constructor args
         *
         * TEMPLATE SYNTAX:
         *   template<typename T>
         *
         * T is a "type parameter" - the caller specifies what type to use:
         *   RegisterTest<MyTest>("My Test");  // T = MyTest
         *
         * LAMBDA AS FACTORY:
         *   []() -> Tests* { return new T(); }
         *
         * This creates an anonymous function (lambda) that:
         * - [] = captures nothing from surrounding scope
         * - () = takes no parameters
         * - -> Tests* = returns a Tests pointer
         * - { return new T(); } = body creates new instance of T
         *
         * The lambda is stored and called later when the user selects this test.
         *
         * std::make_pair creates a pair<string, function<Tests*()>> which is
         * pushed into the m_Tests vector.
         */
        template<typename T>
        void RegisterTest(const std::string& name)
        {
            m_Tests.push_back(std::make_pair(name, []() -> Tests* { return new T(); }));
        }

        // ADVANCED REGISTRATION (With Constructor Arguments)
        /**
         * RegisterTest<T, Args...>(name, args...) - Register test with constructor args
         *
         * This is where the crazy code lives. Let's break it down piece by piece.
         *
         * VARIADIC TEMPLATES (typename... Args)
         * The "..." creates a PARAMETER PACK - zero or more template arguments.
         *
         *   RegisterTest<TestFoo>("Foo", 42, 3.14f, "hello");
         *   // Args = {int, float, const char*}
         *   // args = {42, 3.14f, "hello"}
         * 
         * PERFECT FORWARDING (std::forward<Args>(args         
         * Preserves the VALUE CATEGORY (lvalue vs rvalue) of each argument.
         *
         * Problem it solves:
         *   void wrapper(int& x) { foo(x); }      // Only accepts lvalues
         *   void wrapper(int&& x) { foo(x); }     // Only accepts rvalues
         *   // Need both overloads? That's 2^N functions for N parameters!
         *
         * Solution - Universal/Forwarding references (Args&&):
         *   template<typename T>
         *   void wrapper(T&& x) { foo(std::forward<T>(x)); }
         *   // Single function handles both lvalues and rvalues correctly!
         *
         * std::forward<T>(x) returns:
         *   - T& if x was an lvalue (passed as lvalue)
         *   - T&& if x was an rvalue (passed as rvalue, enabling move semantics)
         *
         * CAPTURING ARGUMENTS IN LAMBDA is The Tricky Part
         * Problem: We need to STORE the arguments now, but USE them later.
         *
         *   // This WON'T work - args is a parameter pack, can't capture directly
         *   [args...]() { return new T(args...); }  // ERROR!
         *
         * Solution: Pack arguments into a tuple, capture the tuple.
         *
         *   [args = std::make_tuple(std::forward<Args>(args)...)]()
         *
         * This uses C++14 INIT-CAPTURE:
         *   [args = expression]  // Creates new variable 'args' initialized to expression
         *
         * std::make_tuple(a, b, c) creates std::tuple<A, B, C> holding copies of a, b, c.
         * The forward<Args>(args)... expands to forward each argument individually:
         *   std::make_tuple(std::forward<int>(42), std::forward<float>(3.14f), ...)
         *
         * -------------------------------------------------------------------------
         * UNPACKING THE TUPLE (std::apply)
         * -------------------------------------------------------------------------
         * Now we have a tuple, but new T() needs individual arguments. std::apply to the rescue!
         *
         *   std::apply(callable, tuple)
         *
         * Calls callable with tuple elements unpacked as arguments:
         *   std::apply(foo, std::tuple{1, 2, 3})  // Equivalent to: foo(1, 2, 3)
         *
         * -------------------------------------------------------------------------
         * THE NESTED LAMBDA
         * -------------------------------------------------------------------------
         *   std::apply([](auto&&... unpackedArgs) {
         *       return new T(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
         *   }, args);
         *
         * - auto&&... unpackedArgs: Another parameter pack, types deduced from tuple
         * - decltype(unpackedArgs): Gets the type of each unpacked argument
         * - std::forward<decltype(...)>(...): Perfect forwards each unpacked arg
         * - new T(...): Finally constructs T with all the original arguments!
         *
         * -------------------------------------------------------------------------
         * FULL EXECUTION FLOW
         * -------------------------------------------------------------------------
         *
         *   1. RegisterTest<TestFoo>("Foo", 42, "hello") is called
         *
         *   2. A lambda is created that captures tuple{42, "hello"}:
         *      [args = tuple{42, "hello"}]() -> Tests* { ... }
         *
         *   3. Lambda is stored in m_Tests vector
         *
         *   4. Later, user clicks "Foo" in menu...
         *
         *   5. Factory lambda is called:
         *      - std::apply unpacks tuple into (42, "hello")
         *      - Inner lambda receives (auto&& a, auto&& b) = (int&&, const char*&&)
         *      - new TestFoo(42, "hello") is called
         *      - Returns the new TestFoo*
         */
        template<typename T, typename... Args>
        void RegisterTest(const std::string& name, Args&&... args)
        {
            m_Tests.push_back(std::make_pair(name,
                // Outer lambda: Captures constructor arguments as a tuple
                // This is stored in m_Tests and called when user selects the test
                [args = std::make_tuple(std::forward<Args>(args)...)]() -> Tests* {
                    // Inner lambda: Receives unpacked tuple elements as parameters
                    // std::apply calls this lambda with the tuple elements as arguments
                    return std::apply([](auto&&... unpackedArgs)
                        {
                            // Finally construct T with all the original arguments,
                            // using perfect forwarding to preserve value categories
                            return new T(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
                        }, args);
                }));
        }

    private:
        /**
         * m_CurrentTest - Reference to external pointer that tracks active test
         * This reference allows TestMenu to change which test is currently running
         * in the main application loop.
         */
        Tests*& m_CurrentTest;

        /**
         * m_Tests - Registry of available tests
         *
         * Each entry is a pair of:
         * - std::string: Display name for the test (shown in menu)
         * - std::function<Tests*()>: Factory function that creates the test
         *
         * std::function<Tests*()> is TYPE-ERASED container for any callable
         * that takes no arguments and returns Tests*. This includes:
         * - Lambda functions
         * - Function pointers
         * - Functors (objects with operator())
         * - std::bind expressions
         *
         * TYPE ERASURE means we lose the concrete type information but gain
         * the ability to store heterogeneous callables in a single container.
         */
        std::vector<std::pair<std::string, std::function<Tests* ()>>> m_Tests;
    };
}
