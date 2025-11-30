#pragma once

#include "../Camera/Camera.h"
#include "../Voxel/Chunk.h"
#include "../Model.h"
#include "glm/glm.hpp"
#include <string>

// Axis-Aligned Bounding Box for collision detection
struct AABB
{
    glm::vec3 min;  // Bottom-left-back corner
    glm::vec3 max;  // Top-right-front corner

    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    // Get AABB centered at position with given size
    static AABB FromCenter(const glm::vec3& center, const glm::vec3& size)
    {
        glm::vec3 halfSize = size * 0.5f;
        return AABB(center - halfSize, center + halfSize);
    }

    glm::vec3 GetCenter() const { return (min + max) * 0.5f; }
    glm::vec3 GetSize() const { return max - min; }
};

// First-person player with physics and collision
class Player
{
public:
    enum class CameraMode
    {
        FirstPerson,
        ThirdPerson
    };

private:
    // Camera for view control (first-person)
    Camera m_Camera;

    // Visual representation
    Model* m_Model;

    // Physics state
    glm::vec3 m_Position;      // Player's feet position
    glm::vec3 m_Velocity;      // Current velocity (blocks/second)

    // Collision box (Minecraft-style: 0.6 wide, 1.8 tall)
    static constexpr float PLAYER_WIDTH = 0.6f;
    static constexpr float PLAYER_HEIGHT = 1.8f;
    static constexpr float PLAYER_DEPTH = 0.6f;
    static constexpr float EYE_HEIGHT = 1.62f;  // Camera offset from feet

    // Physics constants
    static constexpr float GRAVITY = 20.0f;           // Blocks/sec^2 (downward)
    static constexpr float JUMP_VELOCITY = 4.0f;      // Initial upward velocity
    static constexpr float WALK_SPEED = 4.317f;       // Blocks/sec (Minecraft default)
    static constexpr float SPRINT_SPEED = 5.612f;     // Blocks/sec
    static constexpr float TERMINAL_VELOCITY = 50.0f; // Max fall speed
    static constexpr float GROUND_FRICTION = 0.9f;    // Horizontal slowdown

    // State flags
    bool m_IsGrounded;
    bool m_IsSprinting;

    // Camera mode
    CameraMode m_CameraMode;
    static constexpr float THIRD_PERSON_DISTANCE = 5.0f;  // Distance behind player
    static constexpr float THIRD_PERSON_HEIGHT = 2.0f;    // Height above player

public:
    Player(const glm::vec3& startPosition);
    ~Player();

    // Main update function - call every frame
    void Update(float deltaTime, Chunk& chunk);

    // Input handling
    void ProcessMovement(Camera::Movement direction, float deltaTime);
    void ProcessMouseMovement(float xOffset, float yOffset);
    void ProcessMouseScroll(float yOffset);
    void Jump();
    void SetSprinting(bool sprinting) { m_IsSprinting = sprinting; }

    // Camera mode control
    void ToggleCameraMode();
    void SetCameraMode(CameraMode mode) { m_CameraMode = mode; UpdateCameraPosition(); }
    CameraMode GetCameraMode() const { return m_CameraMode; }

    // Model management
    bool LoadModel(const std::string& filepath);

    // Getters
    Camera& GetCamera() { return m_Camera; }
    const Camera& GetCamera() const { return m_Camera; }

    Model* GetModel() { return m_Model; }
    const Model* GetModel() const { return m_Model; }

    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetVelocity() const { return m_Velocity; }
    AABB GetAABB() const;

    bool IsGrounded() const { return m_IsGrounded; }
    bool IsSprinting() const { return m_IsSprinting; }

    // Setters
    void SetPosition(const glm::vec3& position);
    void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; }

private:
    // Physics helpers
    void ApplyGravity(float deltaTime);
    void ApplyFriction(float deltaTime);
    void UpdateCameraPosition();
    void UpdateModelTransform();

    // Collision detection (will be enhanced in Phase 2)
    void ResolveCollision(Chunk& chunk, float deltaTime);
};
