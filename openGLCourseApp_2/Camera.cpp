#include "Camera.h"

Camera::Camera() {
	smoothingFactor = 0.1f;
	smoothedDelta = glm::vec2(0.0f);
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed) {
	smoothingFactor = 0.1f;
	smoothedDelta = glm::vec2(0.0f);
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime) {
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W]) {
		position += front * velocity;
	}
	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}
	if (keys[GLFW_KEY_A]) {
		position -= right * velocity;
	}
	if (keys[GLFW_KEY_D]) {
		position += right * velocity;
	}

}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	// Raw delta (invert Y for FPS mouse-look)
	glm::vec2 rawDelta(xChange, -yChange);

	// Exponential smoothing (LERP)
	smoothedDelta = smoothedDelta + smoothingFactor * (rawDelta - smoothedDelta);

	// Apply sensitivity
	yaw += smoothedDelta.x * turnSpeed;
	pitch += smoothedDelta.y * turnSpeed;

	// Pitch clamp
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	update();
}

void Camera::mouseControlOrbit(GLfloat xChange, GLfloat yChange, GLfloat zoomChange)
{
	// Update angles based on movement
	angleX -= xChange * turnSpeed;
	angleY -= yChange * turnSpeed;

	// Zooming
	distance -= zoomChange;
	if (distance < 1.0f) distance = 1.0f;

	// Clamp vertical angle so we don't flip
	if (angleY > 89.0f) angleY = 89.0f;
	if (angleY < -10.0f) angleY = -10.0f;
}


glm::mat4 Camera::calculateViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::vec3 Camera::calculateOrbitPosition(glm::vec3 target)
{
	// spherical coords
	float theta = glm::radians(angleX);
	float phi = glm::radians(angleY);

	float x = distance * cos(phi) * sin(theta);
	float y = distance * sin(phi);
	float z = distance * cos(phi) * cos(theta);

	position = target + glm::vec3(x, y, z);

	// look at the target
	front = glm::normalize(target - position);
	// calc right and up vecs
	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, front));

	return position;
}

void Camera::update() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

}

Camera::~Camera() {

}