#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightMatrices[6];

out vec4 FragPos;


/*
	This geometry shader is relatively straightforward.
	We take as input a triangle, and output a total of 6 triangles (6 * 3 equals 18 vertices).
	
	In the main function we iterate over 6 cubemap faces where we specify each face as the 
	output face by storing the face integer into gl_Layer.
	
	We then generate the output triangles by transforming each world-space input vertex to the relevant
	light space by multiplying FragPos with the face's light-space transformation matrix.
	
	Note that we also sent the resulting FragPos variable to the fragment shader that we'll need to calculate a depth value.
*/

void main()
{
	// Making a cubeMap
	for(int face = 0; face < 6; face++)
	{
		gl_Layer = face;
		
		for(int i =0; i < 3; i++)
		{
			// gl_in is to take whatever is in => "layout (triangles) in;"
			FragPos = gl_in[i].gl_Position;
			gl_Position = lightMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}

};