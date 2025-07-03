#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(GLFWwindow* window,
		glm::vec3 startPosition,
		glm::vec3 startUp,
		float startYaw,
		float startPitch,
		float startFOV);

	void processInput(float deltaTime);

	glm::mat4 getViewMatrix();

	void Update(float deltaTime);
	float getFOV() const { return fov; }
	glm::vec3 getPosition() const { return position; }
	bool isDetached() const { return detached; }

private:
	GLFWwindow* m_window;

	void ProcessKeyboard(float deltaTime);
	void ProcessMouse(float xOffset, float yOffset);
	void ToggleDetach();
	void Orbit(float angleX, float angleY);

	glm::vec3 position, front, up, right, worldUp;
	glm::vec3 targetPosition;
	glm::vec3 targetFront;
	bool detached = false;

	float yaw, pitch;
	float fov;
	float movementSpeed, mouseSensitivity, smoothFactor;

	void UpdateCameraVectors();
};
