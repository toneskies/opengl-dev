#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
	GLWindow();
	GLWindow(GLint windowWidth, GLint windowHeight);

	int Initialize();

	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~GLWindow();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;
	
	bool keys[1024];

	GLfloat lastX;			// check previous X on mouse 
	GLfloat lastY;			// check previous Y on mouse
	GLfloat xChange;		// compare current to lastX
	GLfloat yChange;		// compare current to lastY
	bool mouseFirstMoved;	// dont flick the screen on program start!

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

};

