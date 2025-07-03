#include "Camera.h"

#include <iostream>

Camera::Camera(GLFWwindow* window, glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch,float startFOV):

	position(startPosition),
	worldUp(startUp),
	yaw(startYaw),
	pitch(startPitch),
	fov(startFOV),
	movementSpeed(5.0f),
	mouseSensitivity(0.1f),
	smoothFactor(0.1f),
	m_window(window)
{
	UpdateCameraVectors();

	targetPosition = position;
	targetFront = front;

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 newFront;

	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(newFront);
	right = glm::normalize(glm::cross(front, worldUp));
	up =    glm::normalize(glm::cross(right, front));
}

void Camera::processInput(float deltaTime)
{
	static bool escapePressed = false;

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escapePressed)
	{
		this->ToggleDetach();
		escapePressed = true;
	} else if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
	{
		escapePressed = false;
	}


	//only going to process camera movement if we are not set to detached
	if (!detached)
	{
		this->ProcessKeyboard(deltaTime);
	}



	static double lastX = 400.0, lastY = 300.0;
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	if (!detached)
	{
		this->ProcessMouse(xOffset, yOffset);
	}
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::Update(float deltaTime)
{
	if (detached)
	{
		position = glm::mix(position, targetPosition, smoothFactor * deltaTime);
		front = glm::mix(front, targetFront, smoothFactor * deltaTime);
	}
}

void Camera::ProcessKeyboard(float deltaTime)
{
	if (detached) return;


	float velocity = movementSpeed * deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) position += front * velocity;
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) position -= front * velocity;
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) position -= right * velocity;
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) position += right * velocity;
}

void Camera::ProcessMouse(float xOffset, float yOffset)
{
	if (detached) return;

	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;


	yaw += xOffset;
	pitch -= yOffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	UpdateCameraVectors();
}

void Camera::ToggleDetach()
{
	detached = !detached;

	std::cout << (detached ? "Camera is detached for GUI use. \n" : "Camera reattached. \n");

	if (detached)
	{
		//enable the cursor so gui can be used
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		targetPosition = position;
		targetFront = front;

	}
	else
	{
		//else detached so hide the cursor for FPS style movement
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void Camera::Orbit(float angleX, float angleY)
{
	float radius = glm::length(position - targetPosition);

	yaw += angleX * mouseSensitivity;
	pitch += angleY * mouseSensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;


	position.x = targetPosition.x + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	position.y = targetPosition.y + radius * sin(glm::radians(pitch));
	position.z = targetPosition.z + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	UpdateCameraVectors();
}

