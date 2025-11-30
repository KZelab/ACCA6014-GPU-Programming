#include <GL/glew.h>  // MUST be included before any OpenGL headers

#include "InputHandler.h"

// Static instance for callbacks
InputHandler* InputHandler::s_Instance = nullptr;

InputHandler::InputHandler(Player* player, Chunk* chunk)
    : m_Player(player)
    , m_Chunk(chunk)
    , m_FirstMouse(true)
    , m_LastX(640.0f)
    , m_LastY(360.0f)
    , m_SpaceWasPressed(false)
    , m_VWasPressed(false)
{
    // Set this as the singleton instance
    s_Instance = this;
}

InputHandler::~InputHandler()
{
    s_Instance = nullptr;
}

void InputHandler::SetupCallbacks(GLFWwindow* window)
{
    // Register GLFW callbacks
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // Capture mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::ProcessInput(GLFWwindow* window, float deltaTime)
{
    if (!m_Player || !m_Chunk)
        return;

    HandleKeyboard(window, deltaTime);
}

void InputHandler::HandleKeyboard(GLFWwindow* window, float deltaTime)
{
    // Close window on ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Horizontal movement (WASD)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_Player->ProcessMovement(Camera::Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_Player->ProcessMovement(Camera::Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_Player->ProcessMovement(Camera::Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_Player->ProcessMovement(Camera::Movement::RIGHT, deltaTime);
    }

    // Jump (only triggers once per press)
    bool spaceIsPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceIsPressed && !m_SpaceWasPressed)
    {
        m_Player->Jump();
    }
    m_SpaceWasPressed = spaceIsPressed;

    // Camera mode toggle (V key - only triggers once per press)
    bool vIsPressed = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;
    if (vIsPressed && !m_VWasPressed)
    {
        m_Player->ToggleCameraMode();
    }
    m_VWasPressed = vIsPressed;

    // Sprint toggle (hold Ctrl)
    m_Player->SetSprinting(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
}

void InputHandler::HandleMouseMovement(float xpos, float ypos)
{
    if (!m_Player)
        return;

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
        return;
    }

    float xOffset = xpos - m_LastX;
    float yOffset = m_LastY - ypos; // Reversed: y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    m_Player->ProcessMouseMovement(xOffset, yOffset);
}

void InputHandler::HandleMouseScroll(float yoffset)
{
    if (m_Player)
    {
        m_Player->ProcessMouseScroll(yoffset);
    }
}

// Static callback functions (forward to instance)
void InputHandler::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (s_Instance)
    {
        s_Instance->HandleMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void InputHandler::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (s_Instance)
    {
        s_Instance->HandleMouseScroll(static_cast<float>(yoffset));
    }
}
