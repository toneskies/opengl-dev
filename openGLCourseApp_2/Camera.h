#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{

public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	
	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	void mouseControlOrbit(GLfloat xChange, GLfloat yChange, GLfloat zoomChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::vec3 calculateOrbitPosition(glm::vec3 target);
	
	glm::mat4 calculateViewMatrix();

	
	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	glm::vec2 smoothedDelta;
	GLfloat smoothingFactor;


	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	GLfloat distance = 10.0f;
	GLfloat angleX = 0.0f;
	GLfloat angleY = 0.0f;

	void update();
};

