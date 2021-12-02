#pragma once

#include <GL/glew.h>

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

	~Mesh();

private:
	// Vertex Array Object
	// Vertex Buffer Object
	// Index Buffer Object (can be called EBO for Elements Buffer Object)
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

