#include "Camera.h"

Camera::Camera() :
	m_CameraPos(glm::vec3(0.0f, 0.0f, 10.0f)),
	m_CameraTarget(glm::vec3(0.0f, 0.0f, 0.0f)),
	m_CameraFront(glm::normalize(m_CameraTarget - m_CameraPos)),
	m_CameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	m_CameraRight(glm::normalize(glm::cross(m_CameraUp, m_CameraFront))),
	u_Yaw(-90.0f),
	u_Pitch(0),
	lastX(0),
	lastY(0),
	m_CameraSpeed(30.0f)
{
	m_CameraUp = glm::cross(m_CameraFront, m_CameraRight);
	m_View = glm::lookAt(m_CameraPos,
		m_CameraTarget,
		m_CameraUp);
}

Camera::~Camera()
{
}

void Camera::ProcessKeyboardInput(float deltaTime, GLFWwindow* window)
{
	float speed = m_CameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_CameraPos += speed * m_CameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_CameraPos -= speed * m_CameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * speed;
}

void Camera::ProcessMouseInput(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= u_MouseSensitivity;
	yoffset *= u_MouseSensitivity;

	u_Yaw += xoffset;
	u_Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (u_Pitch > 89.0f)
			u_Pitch = 89.0f;
		if (u_Pitch < -89.0f)
			u_Pitch = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

glm::mat4 Camera::GetView() const
{
	return glm::lookAt(m_CameraPos,
		m_CameraPos + m_CameraFront,
		m_CameraUp);
}

glm::vec3 Camera::GetPos() const
{
	return m_CameraPos;
}

glm::vec3 Camera::GetFront() const
{
	return m_CameraFront;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(u_Yaw)) * cos(glm::radians(u_Pitch));
	front.y = sin(glm::radians(u_Pitch));
	front.z = sin(glm::radians(u_Yaw)) * cos(glm::radians(u_Pitch));
	m_CameraFront = glm::normalize(front);
	// also re-calculate the Right and Up vector
	m_CameraRight = glm::normalize(glm::cross(m_CameraFront, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));
}


