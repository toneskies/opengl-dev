#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
	indexCount = numOfIndices;


	// Execution summary:
	// 1. Create a VAO (folder)
	// 2. Open the VAO (Bind)
	// 3. Create a VBO (Memory)
	// 4. Fill the VBO with vertex data
	// 5. Tell the VAO how to read that VBO (AttribPointer)
	// 6. Close the VAO
	// Now when drawing this triangle, just need to call glBindVertexArray(VAO) and issue a draw

	glGenVertexArrays(1, &VAO);	// Generates an ID for a VAO

	glBindVertexArray(VAO);	// Binds the VAO, opens the VAO essentially (open file op)

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glGenBuffers(1, &VBO);	// Generates an ID for a buffer (chunk of memory)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Binds the VBO to the GL_ARRAY_BUFFER, plugs this memory chunk into the Array Buffer slot

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);	// Copies the data from vertices into the currently bound buffer on the GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);	// explains to the GPU how to interpret the raw pile of numbers
	glEnableVertexAttribArray(0);		// Enables the attribute index 0, turning on the switch for glVertexAttribPointer to work
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void *)(sizeof(vertices[0]) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbinds the VBO, 0 means No Object, unplug
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);	// Unbinds the VAO, close file essentially


}

void Mesh::RenderMesh() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::ClearMesh() {

	// IMPORTANT!!! Graphics Card will crash if memory overflows...
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh() {
	ClearMesh();
}