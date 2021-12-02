#version 330

// The location are based on the glVertexAttribPointer of the VBO
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

// All these uniform are passed in main.cpp
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 directionalLightTransform;

void main()
{
	// MVP Transformation for the vertices of a mesh
	gl_Position = projection * view * model * vec4(pos, 1.0);
	DirectionalLightSpacePos = directionalLightTransform * model * vec4(pos,1.0);
	vCol = vec4(clamp(pos,0.0f,1.0f),1.0f);
	
	TexCoord = tex;
	
	// We transppose and inverse the model matrix because normal
	// tend to scale wrongly because of the perspective projection
	Normal = mat3(transpose(inverse(model))) * norm;
	
	FragPos = (model * vec4(pos, 1.0)).xyz;
};