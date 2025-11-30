#include "Camera.h"
#include <algorithm>
#include <iostream>

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_Position(position)
    , m_WorldUp(up)
    , m_Yaw(yaw)
    , m_Pitch(pitch)
    , m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_MovementSpeed(10.0f)
    , m_MouseSensitivity(0.1f)
    , m_Zoom(DEFAULT_ZOOM)
{
    UpdateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_Position(glm::vec3(posX, posY, posZ))
    , m_WorldUp(glm::vec3(upX, upY, upZ))
    , m_Yaw(yaw)
    , m_Pitch(pitch)
    , m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_MovementSpeed(10.0f)
    , m_MouseSensitivity(0.1f)
    , m_Zoom(DEFAULT_ZOOM)
{
    UpdateCameraVectors();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) const
{
    return glm::perspective(glm::radians(m_Zoom), aspectRatio, nearPlane, farPlane);
}

void Camera::ProcessKeyboard(Movement direction, float deltaTime)
{
    float velocity = m_MovementSpeed * deltaTime;

    switch (direction)
    {
    case Movement::FORWARD:
        m_Position += m_Front * velocity;
        break;
    case Movement::BACKWARD:
        m_Position -= m_Front * velocity;
        break;
    case Movement::LEFT:
        m_Position -= m_Right * velocity;
        break;
    case Movement::RIGHT:
        m_Position += m_Right * velocity;
        break;
    case Movement::UP:
        m_Position += m_WorldUp * velocity;
        break;
    case Movement::DOWN:
        m_Position -= m_WorldUp * velocity;
        break;
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= m_MouseSensitivity;
    yOffset *= m_MouseSensitivity;

    m_Yaw += xOffset;
    m_Pitch += yOffset;

    // Constrain pitch to prevent camera flipping
    if (constrainPitch)
    {
        m_Pitch = std::clamp(m_Pitch, MIN_PITCH, MAX_PITCH);
    }

    // Update front, right and up vectors
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
    m_Zoom -= yOffset;
    m_Zoom = std::clamp(m_Zoom, MIN_ZOOM, MAX_ZOOM);
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_Position = position;
}

void Camera::SetPosition(float x, float y, float z)
{
    m_Position = glm::vec3(x, y, z);
}

void Camera::SetZoom(float zoom)
{
    m_Zoom = std::clamp(zoom, MIN_ZOOM, MAX_ZOOM);
}

void Camera::GetChunkPosition(int& chunkX, int& chunkZ, int chunkSize) const
{
    // Convert world position to chunk coordinates
    // Use floor division to handle negative coordinates correctly
    chunkX = static_cast<int>(std::floor(m_Position.x / chunkSize));
    chunkZ = static_cast<int>(std::floor(m_Position.z / chunkSize));
}

bool Camera::IsInFrontOfCamera(const glm::vec3& point) const
{
    // Calculate vector from camera to point
    glm::vec3 toPoint = point - m_Position;

    // Check if point is in front of camera (dot product > 0)
    return glm::dot(toPoint, m_Front) > 0.0f;
}

void Camera::debugPrint() const
{
    std::cout << "Camera pos: (" << m_Position.x << ", " << m_Position.y << ", " << m_Position.z << ")" << std::endl;
    std::cout << "Chunk: (" << static_cast<int>(std::floor(m_Position.x / 16)) << ", " << static_cast<int>(std::floor(m_Position.z / 16)) << ")" << std::endl;
}

void Camera::UpdateCameraVectors()
{
    // Calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Recalculate the right and up vectors
    // Normalise the vectors because their length gets closer to 0 the more you look up or down
    // which results in slower movement
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}