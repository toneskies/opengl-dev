#include "GLWindow.h"

GLWindow::GLWindow() {
	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

	mainWindow = nullptr;
	mouseFirstMoved = true;
	xChange = 0.0f;
	yChange = 0.0f;
	lastX = 0.0f;
	lastY = 0.0f;
	bufferWidth = 0;
	bufferHeight = 0;

}



GLWindow::GLWindow(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

	mainWindow = nullptr;
	mouseFirstMoved = true;
	xChange = 0.0f;
	yChange = 0.0f;
	lastX = 0.0f;
	lastY = 0.0f;
	bufferWidth = 0;
	bufferHeight = 0;
}

int GLWindow::Initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		printf("GLFW Initialisation failed!\n");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW Window Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);				   // --> 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);				   //     3.3 <---
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // how opengl will treat the code (no backwards compat)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // allow forward compat

	// Create Window
	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow) {
		printf("GLFW window creation failed!\n");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);		// multiple window support doing this?

	// Handle keys and mouse
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization failed!\n");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Setup Viewport Size
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

void GLWindow::createCallbacks() {
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat GLWindow::getXChange() {
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat GLWindow::getYChange() {
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void GLWindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
	GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
		}
	}
}

void GLWindow::handleMouse(GLFWwindow* window, double xPos, double yPos) {
	GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved) {
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;		// y is +y downwards, flip to keep convention

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

}

GLWindow::~GLWindow() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}