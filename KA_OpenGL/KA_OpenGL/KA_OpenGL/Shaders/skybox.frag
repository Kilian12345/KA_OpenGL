#version 330

layout (location = 0) in vec3 pos;

in vec3 TexCoords;

out vec4 color;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox, TexCoords);
};