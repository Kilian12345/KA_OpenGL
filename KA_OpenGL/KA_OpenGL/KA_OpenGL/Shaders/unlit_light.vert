#version 330

// The location are based on the glVertexAttribPointer of the VBO
layout (location = 0) in vec3 pos;

out vec4 vCol;
out vec3 FragPos;

// All these uniform are passed in main.cpp
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec4 baseColor;

void main()
{
	// MVP Transformation for the vertices of a mesh
	gl_Position = projection * view * model * vec4(pos, 1.0);
	//vCol = vec4(clamp(pos,0.0f,1.0f),1.0f);
	vCol = baseColor;
	
	FragPos = (model * vec4(pos, 1.0)).xyz;
};