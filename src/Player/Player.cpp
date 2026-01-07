#include <GL/glew.h>  // MUST be included before any OpenGL headers
#include <GLFW/glfw3.h>

#include "Player.h"
#include "../OBJLoader.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Player::Player(const glm::vec3& startPosition)
    : m_Camera(startPosition + glm::vec3(0.0f, EYE_HEIGHT, 0.0f))
    , m_Model(nullptr)
    , m_Position(startPosition)
    , m_Velocity(0.0f)
    , m_IsGrounded(false)
    , m_IsSprinting(false)
    , m_CameraMode(CameraMode::FirstPerson)
{
}

Player::~Player()
{
    if (m_Model)
    {
        delete m_Model;
        m_Model = nullptr;
    }
}

void Player::Update(float deltaTime, Chunk& chunk)
{
    // Apply physics
    ApplyGravity(deltaTime);

    // Apply friction when grounded
    if (m_IsGrounded)
    {
        ApplyFriction(deltaTime);
    }

    // Calculate new position based on velocity
    glm::vec3 newPosition = m_Position + m_Velocity * deltaTime;

    // Collision detection and response
    ResolveCollision(chunk, deltaTime);

    // Update camera to player's eye position
    UpdateCameraPosition();

    // Update model position/rotation
    UpdateModelTransform();
}

void Player::ProcessMovement(Camera::Movement direction, float deltaTime)
{
    // Get camera direction vectors (for horizontal movement)
    glm::vec3 front = m_Camera.GetFront();
    glm::vec3 right = m_Camera.GetRight();

    // Zero out Y component for horizontal-only movement
    front.y = 0.0f;
    right.y = 0.0f;

    if (glm::length(front) > 0.0f)
        front = glm::normalize(front);
    if (glm::length(right) > 0.0f)
        right = glm::normalize(right);

    // Determine movement speed
    float speed = m_IsSprinting ? SPRINT_SPEED : WALK_SPEED;
    float velocity = speed * deltaTime;

    // Apply horizontal movement to velocity (not position directly)
    glm::vec3 horizontalVelocity(0.0f);

    switch (direction)
    {
    case Camera::Movement::FORWARD:
        horizontalVelocity = front * speed;
        break;
    case Camera::Movement::BACKWARD:
        horizontalVelocity = -front * speed;
        break;
    case Camera::Movement::LEFT:
        horizontalVelocity = -right * speed;
        break;
    case Camera::Movement::RIGHT:
        horizontalVelocity = right * speed;
        break;
    default:
        break;
    }

    // Set horizontal velocity (preserve vertical velocity from gravity/jumping)
    m_Velocity.x = horizontalVelocity.x;
    m_Velocity.z = horizontalVelocity.z;
}

void Player::ProcessMouseMovement(float xOffset, float yOffset)
{
    m_Camera.ProcessMouseMovement(xOffset, yOffset);
}

void Player::ProcessMouseScroll(float yOffset)
{
    m_Camera.ProcessMouseScroll(yOffset);
}

void Player::Jump()
{
    // Can only jump when grounded
    if (m_IsGrounded)
    {
        m_Velocity.y = JUMP_VELOCITY;
        m_IsGrounded = false;  // No longer grounded after jumping
        std::cout << "Jump! Velocity: " << m_Velocity.y << std::endl;
    }
    else
    {
        std::cout << "Cannot jump - not grounded! Position Y: " << m_Position.y << std::endl;
    }
}

AABB Player::GetAABB() const
{
    // Create AABB centered at player position (feet)
    glm::vec3 center = m_Position + glm::vec3(0.0f, PLAYER_HEIGHT * 0.5f, 0.0f);
    glm::vec3 size(PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_DEPTH);
    return AABB::FromCenter(center, size);
}

void Player::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    UpdateCameraPosition();
}

void Player::ApplyGravity(float deltaTime)
{
    // Apply downward acceleration (unless on ground)
    if (!m_IsGrounded)
    {
        m_Velocity.y -= GRAVITY * deltaTime;

        // Clamp to terminal velocity
        if (m_Velocity.y < -TERMINAL_VELOCITY)
        {
            m_Velocity.y = -TERMINAL_VELOCITY;
        }
    }
}

void Player::ApplyFriction(float deltaTime)
{
    // Apply horizontal friction when grounded
    m_Velocity.x *= GROUND_FRICTION;
    m_Velocity.z *= GROUND_FRICTION;

    // Stop completely if velocity is very small
    if (std::abs(m_Velocity.x) < 0.01f) m_Velocity.x = 0.0f;
    if (std::abs(m_Velocity.z) < 0.01f) m_Velocity.z = 0.0f;
}

void Player::UpdateCameraPosition()
{
    if (m_CameraMode == CameraMode::FirstPerson)
    {
        // First-person: Camera at eye height
        m_Camera.SetPosition(m_Position.x, m_Position.y + EYE_HEIGHT, m_Position.z);
    }
    else // ThirdPerson
    {
        // Third-person: Camera behind and above player
        glm::vec3 cameraFront = m_Camera.GetFront();
        glm::vec3 cameraOffset = -cameraFront * THIRD_PERSON_DISTANCE;

        // Keep camera above player
        glm::vec3 cameraPos = m_Position + glm::vec3(0.0f, PLAYER_HEIGHT + THIRD_PERSON_HEIGHT, 0.0f) + cameraOffset;

        m_Camera.SetPosition(cameraPos.x, cameraPos.y, cameraPos.z);
    }
}

void Player::ResolveCollision(Chunk& chunk, float deltaTime)
{
    // Block-level collision detection with single chunk
    // TODO: Extend to multi-chunk when world system is implemented

    // Apply velocity to position
    glm::vec3 newPosition = m_Position + m_Velocity * deltaTime;

    // Calculate voxel coordinates player occupies
    int minX = static_cast<int>(std::floor(newPosition.x - PLAYER_WIDTH / 2.0f));
    int maxX = static_cast<int>(std::floor(newPosition.x + PLAYER_WIDTH / 2.0f));
    int minZ = static_cast<int>(std::floor(newPosition.z - PLAYER_DEPTH / 2.0f));
    int maxZ = static_cast<int>(std::floor(newPosition.z + PLAYER_DEPTH / 2.0f));
    int feetY = static_cast<int>(std::floor(newPosition.y));

    m_IsGrounded = false;

    // ===== Vertical (Y-axis) collision =====
    // Check ground collision by scanning from current position down to new position
    // This prevents falling through blocks when deltaTime is large

    if (m_Velocity.y < 0.0f)  // Only check when falling
    {
        // Swept collision: check from current position down to target
        int startY = static_cast<int>(std::floor(m_Position.y));
        int endY = static_cast<int>(std::floor(newPosition.y));

        // Scan each Y level from current down to target
        for (int checkY = startY - 1; checkY >= endY - 1; --checkY)
        {
            bool foundGround = false;

            for (int x = minX; x <= maxX; ++x)
            {
                for (int z = minZ; z <= maxZ; ++z)
                {
                    int localX = x - chunk.GetWorldX();
                    int localZ = z - chunk.GetWorldZ();

                    if (Chunk::IsValidPosition(localX, checkY, localZ))
                    {
                        const Voxel& voxel = chunk.GetVoxel(localX, checkY, localZ);

                        if (voxel.IsSolid())
                        {
                            // Hit ground - calculate exact landing position
                            float blockTop = static_cast<float>(checkY + 1);

                            // Only snap if we're actually going to penetrate
                            if (newPosition.y <= blockTop)
                            {
                                m_IsGrounded = true;
                                newPosition.y = blockTop;
                                m_Velocity.y = 0.0f;
                                foundGround = true;
                            }
                            break;
                        }
                    }
                }
                if (foundGround) break;
            }
            if (foundGround) break;
        }
    }
    else  // Moving up or stationary - just check current position
    {
        int checkY = feetY - 1;
        for (int x = minX; x <= maxX; ++x)
        {
            for (int z = minZ; z <= maxZ; ++z)
            {
                int localX = x - chunk.GetWorldX();
                int localZ = z - chunk.GetWorldZ();

                if (Chunk::IsValidPosition(localX, checkY, localZ))
                {
                    const Voxel& voxel = chunk.GetVoxel(localX, checkY, localZ);
                    if (voxel.IsSolid())
                    {
                        m_IsGrounded = true;
                        if (m_Velocity.y <= 0.0f)
                        {
                            newPosition.y = static_cast<float>(checkY + 1);
                            m_Velocity.y = 0.0f;
                        }
                    }
                }
            }
        }
    }

    // Debug output (print once per second)
    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer >= 1.0f)
    {
        std::cout << "Player Y: " << m_Position.y << " | Grounded: " << (m_IsGrounded ? "YES" : "NO")
                  << " | Velocity Y: " << m_Velocity.y << std::endl;
        debugTimer = 0.0f;
    }

    // Check ceiling collision (head hitting block above)
    int headY = static_cast<int>(std::floor(newPosition.y + PLAYER_HEIGHT));
    for (int x = minX; x <= maxX; ++x)
    {
        for (int z = minZ; z <= maxZ; ++z)
        {
            int localX = x - chunk.GetWorldX();
            int localZ = z - chunk.GetWorldZ();

            if (Chunk::IsValidPosition(localX, headY, localZ))
            {
                const Voxel& voxel = chunk.GetVoxel(localX, headY, localZ);

                if (voxel.IsSolid())
                {
                    // Hit ceiling - stop upward movement
                    if (m_Velocity.y > 0.0f)
                    {
                        newPosition.y = static_cast<float>(headY) - PLAYER_HEIGHT;
                        m_Velocity.y = 0.0f;
                    }
                }
            }
        }
    }

    // ===== Horizontal (X-Z) collision =====
    // Check wall collisions at player's body height (check multiple Y levels)
    for (int yOffset = 0; yOffset < static_cast<int>(PLAYER_HEIGHT); ++yOffset)
    {
        int checkY = feetY + yOffset;

        // Check X-axis walls
        int checkX = static_cast<int>(std::floor(newPosition.x + (m_Velocity.x > 0 ? PLAYER_WIDTH / 2.0f : -PLAYER_WIDTH / 2.0f)));
        for (int z = minZ; z <= maxZ; ++z)
        {
            int localX = checkX - chunk.GetWorldX();
            int localZ = z - chunk.GetWorldZ();

            if (Chunk::IsValidPosition(localX, checkY, localZ))
            {
                const Voxel& voxel = chunk.GetVoxel(localX, checkY, localZ);
                if (voxel.IsSolid())
                {
                    // Wall collision - stop horizontal movement in X
                    newPosition.x = m_Position.x;
                    m_Velocity.x = 0.0f;
                    break;
                }
            }
        }

        // Check Z-axis walls
        int checkZ = static_cast<int>(std::floor(newPosition.z + (m_Velocity.z > 0 ? PLAYER_DEPTH / 2.0f : -PLAYER_DEPTH / 2.0f)));
        for (int x = minX; x <= maxX; ++x)
        {
            int localX = x - chunk.GetWorldX();
            int localZ = checkZ - chunk.GetWorldZ();

            if (Chunk::IsValidPosition(localX, checkY, localZ))
            {
                const Voxel& voxel = chunk.GetVoxel(localX, checkY, localZ);
                if (voxel.IsSolid())
                {
                    // Wall collision - stop horizontal movement in Z
                    newPosition.z = m_Position.z;
                    m_Velocity.z = 0.0f;
                    break;
                }
            }
        }
    }

    // Update position after collision resolution
    m_Position = newPosition;
}

bool Player::LoadModel(const std::string& filepath)
{
    std::cout << "\nLoading player model from: " << filepath << std::endl;

    // Delete existing model if any
    if (m_Model)
    {
        delete m_Model;
        m_Model = nullptr;
    }

    // Load model with progress callback
    m_Model = OBJLoader::LoadFromFile(filepath,
        [](size_t current, size_t total, const std::string& status) {
            // Progress callback - print every 500k lines
            if (current % 500000 == 0)
            {
                std::cout << "  Loaded " << (current / 1000000.0f) << "M lines... " << status << std::endl;
            }
        });

    if (!m_Model)
    {
        std::cerr << "ERROR: Failed to load player model!" << std::endl;
        return false;
    }

    // Center the model at its base (feet at Y=0, centered horizontally)
    // This ensures rotation happens around the correct pivot point
    std::cout << "Centering model at base..." << std::endl;
    m_Model->CenterModelAtBase();

    std::cout << "Uploading model to GPU..." << std::endl;
    m_Model->UploadToGPU();

    // Scale the model to match player height (1.8 blocks)
    glm::vec3 modelSize = m_Model->GetBoundsSize();
    float scaleToHeight = PLAYER_HEIGHT / modelSize.y;
    m_Model->SetScale(scaleToHeight);

    std::cout << "Model scaled by " << scaleToHeight << " (original height: " << modelSize.y << ")" << std::endl;
    std::cout << "Player model loaded successfully!" << std::endl;

    // Try to load texture (check common locations)
    std::string modelDir = filepath.substr(0, filepath.find_last_of("/\\") + 1);

    // Try multiple texture paths
    std::vector<std::string> texturePaths = {
        modelDir + "Separate_assets_obj/Textures_4.png",
        modelDir + "Textures_4.png",
        modelDir + "texture.png"
    };

    bool textureLoaded = false;
    for (const auto& texPath : texturePaths)
    {
        if (m_Model->LoadTexture(texPath))
        {
            textureLoaded = true;
            break;
        }
    }

    if (!textureLoaded)
    {
        std::cout << "No texture found - model will use solid color" << std::endl;
    }

    // Initialize model transform
    UpdateModelTransform();

    return true;
}

void Player::UpdateModelTransform()
{
    if (!m_Model)
        return;

    // Position model at player's feet
    m_Model->SetPosition(m_Position);

    // Rotate model to match camera direction (character faces where camera looks)
    // In third-person, you'll see the character turn as you move the mouse
    // Add 180-degree offset to align model's forward with camera's forward direction
    float cameraYaw = m_Camera.GetYaw();
    float modelYaw = cameraYaw + 180.0f;
    m_Model->SetRotation(glm::vec3(0.0f, modelYaw, 0.0f));
}

void Player::ToggleCameraMode()
{
    if (m_CameraMode == CameraMode::FirstPerson)
    {
        m_CameraMode = CameraMode::ThirdPerson;
        std::cout << "Camera: Third Person" << std::endl;
    }
    else
    {
        m_CameraMode = CameraMode::FirstPerson;
        std::cout << "Camera: First Person" << std::endl;
    }

    UpdateCameraPosition();
}
