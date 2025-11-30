#pragma once
#include <GL/glew.h>


class Mesh
{
private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
	GLenum drawMode;
public:

	Mesh();
	~Mesh();

	void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();
	void SetDrawMode(GLenum mode);
};

