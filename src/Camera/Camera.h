#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// First-person camera for navigating the voxel world
class Camera
{
public:
    // Camera movement directions
    enum class Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

private:
    // Camera position and orientation
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // Euler angles
    float m_Yaw;
    float m_Pitch;

    // Camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;

    // Field of view limits
    static constexpr float MIN_ZOOM = 1.0f;
    static constexpr float MAX_ZOOM = 90.0f;
    static constexpr float DEFAULT_ZOOM = 45.0f;

    // Pitch limits (prevent camera flipping)
    static constexpr float MIN_PITCH = -89.0f;
    static constexpr float MAX_PITCH = 89.0f;

    // Update camera vectors based on euler angles
    void UpdateCameraVectors();

public:
    // Constructor with default values
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    );

    // Constructor with scalar values
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    );

    ~Camera();

    // Get view matrix
    glm::mat4 GetViewMatrix() const;

    // Get projection matrix
    glm::mat4 GetProjectionMatrix(float aspectRatio, float nearPlane = 0.1f, float farPlane = 1000.0f) const;

    // Process keyboard input
    void ProcessKeyboard(Movement direction, float deltaTime);

    // Process mouse movement
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    // Process mouse scroll (for zoom/FOV)
    void ProcessMouseScroll(float yOffset);

    // Getters
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetFront() const { return m_Front; }
    glm::vec3 GetUp() const { return m_Up; }
    glm::vec3 GetRight() const { return m_Right; }
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    float GetZoom() const { return m_Zoom; }

    float GetMovementSpeed() const { return m_MovementSpeed; }
    float GetMouseSensitivity() const { return m_MouseSensitivity; }

    // Setters
    void SetPosition(const glm::vec3& position);
    void SetPosition(float x, float y, float z);

    void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
    void SetZoom(float zoom);

    // Utility
    // Get the chunk position the camera is currently in
    void GetChunkPosition(int& chunkX, int& chunkZ, int chunkSize = 16) const;

    // Check if a point is in front of the camera (for frustum culling)
    bool IsInFrontOfCamera(const glm::vec3& point) const;

	void debugPrint() const;    
};