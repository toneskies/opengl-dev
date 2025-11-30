#include "GLWindow.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLWindow::GLWindow() : width(800), height(600), xChange(0), yChange(0),
lastX(0), lastY(0), mouseFirstMoved(true)
{
	for (int i = 0; i < 1024; ++i) keys[i] = false;
}

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight)
	: width(windowWidth), height(windowHeight), xChange(0), yChange(0),
	lastX(0), lastY(0), mouseFirstMoved(true)
{
	for (int i = 0; i < 1024; ++i) keys[i] = false;
}

int GLWindow::Initialize()
{
	if (!glfwInit()) { printf("GLFW init failed!\n"); return 1; }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "GL Window", nullptr, nullptr);
	if (!mainWindow) { printf("GLFW window creation failed!\n"); glfwTerminate(); return 1; }

	glfwMakeContextCurrent(mainWindow);
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(mainWindow, this);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { printf("GLEW init failed!\n"); return 1; }

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, bufferWidth, bufferHeight);

	return 0;
}

void GLWindow::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat GLWindow::getXChange() { GLfloat change = xChange; xChange = 0; return change; }
GLfloat GLWindow::getYChange() { GLfloat change = yChange; yChange = 0; return change; }

void GLWindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) theWindow->keys[key] = (action == GLFW_PRESS);
}

void GLWindow::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
	if (theWindow->mouseFirstMoved) { theWindow->lastX = xPos; theWindow->lastY = yPos; theWindow->mouseFirstMoved = false; }

	theWindow->xChange += xPos - theWindow->lastX;
	theWindow->yChange += yPos - theWindow->lastY;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

void GLWindow::updateCursorMode()
{
	glfwSetInputMode(mainWindow, GLFW_CURSOR, keys[GLFW_KEY_LEFT_CONTROL] ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool GLWindow::RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
	const glm::vec3& sphereCenter, float radius)
{
	glm::vec3 L = sphereCenter - rayOrigin;
	float tca = glm::dot(L, rayDir);
	if (tca < 0) return false;
	float d2 = glm::dot(L, L) - tca * tca;
	return d2 <= radius * radius;
}

void GLWindow::ProcessClick(double mouseX, double mouseY,
	const glm::mat4& projection,
	const glm::mat4& view,
	Camera& camera,
	std::vector<glm::vec3>& trussNodes,
	std::vector<glm::vec3>& nodeColors)
{
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	glm::vec4 rayClip(x, y, -1.0f, 1.0f);

	glm::vec4 rayEye = glm::inverse(projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
	glm::vec3 rayOrigin = camera.getCameraPosition();

	float nodeRadius = 0.25f;
	for (size_t i = 0; i < trussNodes.size(); ++i) {
		if (RayIntersectsSphere(rayOrigin, rayWorld, trussNodes[i], nodeRadius)) {
			nodeColors[i] = (nodeColors[i] == glm::vec3(1.0f)) ? glm::vec3(1.0f, 0, 0) : glm::vec3(1.0f);
		}
	}
}

GLWindow::~GLWindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
