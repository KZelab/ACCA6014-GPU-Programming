
#pragma once

#include <GLFW/glfw3.h>
#include "Player.h"

// Handles all keyboard and mouse input for the player
class InputHandler
{
private:
    Player* m_Player;
    Chunk* m_Chunk;

    // Mouse state
    bool m_FirstMouse;
    float m_LastX;
    float m_LastY;

    // Keyboard state (for single-press detection)
    bool m_SpaceWasPressed;
    bool m_VWasPressed;  // For camera toggle

    // Singleton instance for GLFW callbacks
    static InputHandler* s_Instance;

public:
    InputHandler(Player* player, Chunk* chunk);
    ~InputHandler();

    // Initialize GLFW callbacks
    void SetupCallbacks(GLFWwindow* window);

    // Process input each frame
    void ProcessInput(GLFWwindow* window, float deltaTime);

    // GLFW callback functions (static, forward to instance)
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    // Instance methods for handling input
    void HandleMouseMovement(float xpos, float ypos);
    void HandleMouseScroll(float yoffset);
    void HandleKeyboard(GLFWwindow* window, float deltaTime);
};
