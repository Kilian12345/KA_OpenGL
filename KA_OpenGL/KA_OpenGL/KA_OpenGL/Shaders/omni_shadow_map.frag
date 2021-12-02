#version 330

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

out vec4 color;

void main()
{
	float distance = length(FragPos.xyz - lightPos);
	distance /= farPlane;
	
	// Available only in the fragment language, gl_FragDepth is an output variable
	// that is used to establish the depth value for the current fragment.  
	gl_FragDepth = distance;
};