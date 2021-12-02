#include "Texture.h"

Texture::Texture()
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}

Texture::Texture(const char* fileLoc)
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;
}


bool Texture::LoadTexture()
{
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);

	if (!texData)
	{
		printf("failed to find : %s\n", fileLocation);
		return false;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// GL_REPEAT = Set the texture Wrapping ( here it is in repeat )
	// GL_TEXTURE_WRAP_S = Wrap on x ( here it is in repeat )
	// S = X
	// T = Y
	// R = Z
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set the texture filtering ( Linear, nearest )
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Import texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind texture to be sure to not write on a used texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear the Data
	stbi_image_free(texData);

	return true;
}

bool Texture::LoadTextureA()
{
	// RGBA version of LoadTexture

	// 1 char == 1 byte == lightweight
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);

	if (!texData)
	{
		printf("failed to find : %s\n", fileLocation);
		return false;
	}

	// Generating a texture, more like generating an ID
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set the texture Wrapping ( here it is in repeat )
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set the texture filtering ( Linear, nearest )
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Setup texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear the Data
	stbi_image_free(texData);

	return true;
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE1);
	// Binding this texture to "GL_TEXTURE0"
	glBindTexture(GL_TEXTURE_2D, textureID);
}



Texture::~Texture()
{
	ClearTexture();
}
