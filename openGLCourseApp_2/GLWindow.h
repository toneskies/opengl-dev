#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <vector>
#include <glm/glm.hpp>

class GLWindow
{
public:
	GLWindow();
	GLWindow(GLint windowWidth, GLint windowHeight);

	int Initialize();

	GLfloat getBufferWidth() const { return bufferWidth; }
	GLfloat getBufferHeight() const { return bufferHeight; }

	bool getShouldClose() const { return glfwWindowShouldClose(mainWindow); }

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	bool isCtrlPressed() const { return keys[GLFW_KEY_LEFT_CONTROL]; }

	void swapBuffers() { glfwSwapBuffers(mainWindow); }
	void updateCursorMode();

	// Raycasting click
	void ProcessClick(double mouseX, double mouseY,
		const glm::mat4& projection,
		const glm::mat4& view,
		Camera& camera,
		std::vector<glm::vec3>& trussNodes,
		std::vector<glm::vec3>& nodeColors);

	~GLWindow();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

	// Ray-sphere intersection
	static bool RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
		const glm::vec3& sphereCenter, float radius);
};
