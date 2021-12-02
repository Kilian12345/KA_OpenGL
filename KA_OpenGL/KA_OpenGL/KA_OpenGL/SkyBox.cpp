#include "SkyBox.h"

SkyBox::SkyBox()
{
}

SkyBox::SkyBox(std::vector<std::string> faceLocations)
{
	// Shader Setup
	skyShader = new Shader();
	skyShader->CreateFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();

	// Texture Setup
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, bitDepth;

	for (size_t i = 0; i < 6; i++)
	{
		// 1 char == 1 byte == lightweight
		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);

		if (!texData)
		{
			printf("failed to find : %s\n", faceLocations[i].c_str());
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		// Clear the Data
		stbi_image_free(texData);
	}

	// Set the texture Wrapping ( here it is in GL_CLAMP )
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	// Set the texture filtering ( Linear, nearest )
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Mesh Setup
	unsigned int skyboxIndices[] = {
		//front
		0,1,2,
		2,1,3,
		//right
		2,3,5,
		5,3,7,
		//back
		5,7,4,
		4,7,6,
		//left
		4,6,0,
		0,6,1,
		//top
		5,4,2,
		4,0,2,
		//bottom
		1,6,3,
		3,6,7
	};

	float skyboxVertices[] = {
		// Pos x, y, z			UV x, y				Normals x, y, z
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
												 	  
		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);
}

void SkyBox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	//glDepthMask specifies whether the depth buffer is enabled for writing
	glDepthMask(GL_FALSE);

	skyShader->UseShader();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix)); // Moving the camera

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	skyShader->Validate();

	skyMesh->RenderMesh();

	glDepthMask(GL_TRUE);
}

SkyBox::~SkyBox()
{
}
