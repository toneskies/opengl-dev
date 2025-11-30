#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "CommonValues.h"
#include "PointLight.h"
#include "SpotLight.h"

// ----------------------------
// Global Variables
// ----------------------------
GLWindow mainWindow;
Camera camera;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Texture plainTexture;
Material shinyMaterial;
Material dullMaterial;
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Truss Data
std::vector<unsigned int> trussIndices;
std::vector<GLfloat> trussVertices;
std::vector<glm::vec3> trussNodePositions;
std::vector<glm::vec3> nodeColors;

// Shader files
static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

// Uniforms
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0;
GLuint uniformSpecularIntensity = 0, uniformShininess = 0, uniformObjectColor = 0;

// ----------------------------
// Helper Functions
// ----------------------------
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices,
	unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3) {
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++) {
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

// Ray-Sphere intersection
bool RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
	const glm::vec3& sphereCenter, float radius)
{
	glm::vec3 L = sphereCenter - rayOrigin;
	float tca = glm::dot(L, rayDir);
	if (tca < 0) return false;
	float d2 = glm::dot(L, L) - tca * tca;
	return d2 <= radius * radius;
}

// ----------------------------
// Truss Loader
// ----------------------------
void LoadTruss(const char* fileName, std::vector<unsigned int>& indices, std::vector<GLfloat>& vertices) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		printf("Failed to open truss file: %s\n", fileName);
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') continue;
		std::stringstream ss(line);
		std::string prefix;
		ss >> prefix;
		if (prefix == "v") {
			float x, y, z;
			ss >> x >> y >> z;
			vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
			vertices.push_back(0.0f); vertices.push_back(0.0f);
			vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
		}
		else if (prefix == "l") {
			unsigned int idx1, idx2;
			ss >> idx1 >> idx2;
			indices.push_back(idx1);
			indices.push_back(idx2);
		}
	}
	printf("Loaded Truss: %d vertices, %d indices.\n", (int)vertices.size() / 8, (int)indices.size());
}

// ----------------------------
// Unit Beam
// ----------------------------
void CreateUnitBeam() {
	GLfloat vertices[] = {
		// Front face
		-0.5f, -0.5f,  0.0f, 0,0, 0,0,1,
		 0.5f, -0.5f,  0.0f, 1,0, 0,0,1,
		 0.5f,  0.5f,  0.0f, 1,1, 0,0,1,
		-0.5f,  0.5f,  0.0f, 0,1, 0,0,1,
		// Back
		-0.5f, -0.5f, -1.0f,0,0,0,0,-1,
		 0.5f, -0.5f, -1.0f,1,0,0,0,-1,
		 0.5f,  0.5f, -1.0f,1,1,0,0,-1,
		-0.5f,  0.5f, -1.0f,0,1,0,0,-1,
		// Left
		-0.5f, -0.5f, -1.0f,0,0,-1,0,0,
		-0.5f, -0.5f,  0.0f,1,0,-1,0,0,
		-0.5f,  0.5f,  0.0f,1,1,-1,0,0,
		-0.5f,  0.5f, -1.0f,0,1,-1,0,0,
		// Right
		 0.5f, -0.5f, 0.0f,0,0,1,0,0,
		 0.5f, -0.5f,-1.0f,1,0,1,0,0,
		 0.5f,  0.5f,-1.0f,1,1,1,0,0,
		 0.5f,  0.5f, 0.0f,0,1,1,0,0,
		 // Top
		 -0.5f,0.5f,0,0,0,0,1,0,
		  0.5f,0.5f,0,1,0,0,1,0,
		  0.5f,0.5f,-1,1,1,0,1,0,
		 -0.5f,0.5f,-1,0,1,0,1,0,
		 // Bottom
		 -0.5f,-0.5f,-1,0,0,0,-1,0,
		  0.5f,-0.5f,-1,1,0,0,-1,0,
		  0.5f,-0.5f,0,1,1,0,-1,0,
		 -0.5f,-0.5f,0,0,1,0,-1,0
	};
	unsigned int indices[] = {
		0,1,2,2,3,0, 4,5,6,6,7,4, 8,9,10,10,11,8, 12,13,14,14,15,12, 16,17,18,18,19,16, 20,21,22,22,23,20
	};
	Mesh* beam = new Mesh();
	beam->CreateMesh(vertices, indices, 192, 36);
	meshList.push_back(beam);
}

// ----------------------------
// Unit Sphere
// ----------------------------
void CreateUnitSphere() {
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	const int stackCount = 40, sectorCount = 40;
	const float radius = 1.0f, PI = 3.14159265f;

	for (int i = 0; i <= stackCount; ++i) {
		float stackAngle = PI / 2 - (float)i * PI / stackCount;
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);
		for (int j = 0; j <= sectorCount; ++j) {
			float sectorAngle = (float)j * 2 * PI / sectorCount;
			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);
			vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
			vertices.push_back((float)j / sectorCount); vertices.push_back((float)i / stackCount);
			vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
		}
	}

	for (int i = 0; i < stackCount; ++i) {
		int k1 = i * (sectorCount + 1);
		int k2 = k1 + sectorCount + 1;
		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			if (i != 0) { indices.push_back(k1); indices.push_back(k2); indices.push_back(k1 + 1); }
			if (i != (stackCount - 1)) { indices.push_back(k1 + 1); indices.push_back(k2); indices.push_back(k2 + 1); }
		}
	}

	Mesh* sphere = new Mesh();
	sphere->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	meshList.push_back(sphere);
}

// ----------------------------
// Main
// ----------------------------
int main() {
	// 1. Initialize Window
	mainWindow = GLWindow(1366, 768);
	mainWindow.Initialize();

	// 2. Load Truss
	LoadTruss("truss.txt", trussIndices, trussVertices);

	// 3. Extract node positions
	for (size_t i = 0; i < trussVertices.size(); i += 8) {
		trussNodePositions.push_back(glm::vec3(trussVertices[i], trussVertices[i + 1], trussVertices[i + 2]));
		nodeColors.push_back(glm::vec3(1.0f)); // white default
	}

	// 4. Create Geometry
	CreateUnitBeam();
	CreateUnitSphere();

	// 5. Create Shaders
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	// 6. Camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	// 7. Texture
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	// 8. Materials
	shinyMaterial = Material(0.25f, 16);
	dullMaterial = Material(0.1f, 4);

	// 9. Lights
	mainLight = DirectionalLight(1.0f, 0.95f, 0.85f, 0.05f, 0.4f, -0.2f, -1.0f, -0.3f);
	pointLights[0] = PointLight(glm::vec3(1.0f, 0.9f, 0.7f), 0.05f, 0.3f, glm::vec3(3.0f, 2.0f, 1.0f), 0.1f, 0.09f, 0.032f);
	unsigned int pointLightCount = 1;

	glm::vec3 camPos = camera.getCameraPosition();
	glm::vec3 camDir = camera.getCameraDirection();
	spotLights[0].SetFlash(camPos, camDir);
	spotLights[0] = SpotLight(glm::vec3(1.0f, 0.95f, 0.85f), 0.0f, 0.9f, glm::vec3(camPos.x, camPos.y, camPos.z),
		glm::vec3(camDir.x, camDir.y, camDir.z), 1.0f, 0.09f, 0.032f, 17.5f);
	unsigned int spotLightCount = 1;

	// 10. Projection
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	Mesh* beamMesh = meshList[0];
	Mesh* nodeMesh = meshList[1];

	// Get shader uniform locations
	shaderList[0].UseShader();
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensity();
	uniformShininess = shaderList[0].GetShininessLocation();
	uniformObjectColor = shaderList[0].GetObjectColorLocation();

	// ----------------------------
	// Main Loop
	// ----------------------------
	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		// Camera Orbit only if CTRL pressed
		if (mainWindow.isCtrlPressed()) {
			camera.mouseControlOrbit(mainWindow.getXChange(), mainWindow.getYChange(), 0.0f);
		}

		camera.calculateOrbitPosition(glm::vec3(0.0f));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();

		// Update Flashlight
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		plainTexture.UseTexture();

		// --- RENDER NODES ---
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

		for (size_t i = 0; i < trussNodePositions.size(); ++i) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, trussNodePositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(uniformObjectColor,
				nodeColors[i].r, nodeColors[i].g, nodeColors[i].b);
			nodeMesh->RenderMesh();
		}

		// --- RENDER BEAMS ---
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

		for (size_t i = 0; i < trussIndices.size(); i += 2) {
			unsigned int indexA = trussIndices[i];
			unsigned int indexB = trussIndices[i + 1];
			glm::vec3 p1 = trussNodePositions[indexA];
			glm::vec3 p2 = trussNodePositions[indexB];
			float length = glm::distance(p1, p2);
			if (length < 0.001f) continue;
			glm::mat4 lookAtMat = glm::lookAt(p1, p2, glm::vec3(0, 1, 0));
			glm::mat4 model = glm::inverse(lookAtMat);
			model = glm::scale(model, glm::vec3(0.15f, 0.15f, length));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			beamMesh->RenderMesh();
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
