#include "SpotLight.h"

SpotLight::SpotLight() : PointLight()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	edge = 0;
	procEdge = cosf(glm::radians(edge));
	isOn = true;
}

SpotLight::SpotLight(GLuint shadowWidth, GLuint shadowHeight,
	GLfloat near, GLfloat far,
	GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensity, GLfloat dIntensity,
	GLfloat xPos, GLfloat yPos, GLfloat zPos,
	GLfloat xDir, GLfloat yDir, GLfloat zDir,
	GLfloat con, GLfloat lin, GLfloat exp,
	GLfloat rad, GLfloat edg)
	: PointLight(shadowWidth, shadowHeight, near, far, red,green,blue,aIntensity,dIntensity,xPos,yPos,zPos,con,lin,exp,rad)
{
	direction = glm::normalize(glm::vec3(xDir, yDir, zDir));
	edge = edg;
	procEdge = cosf(glm::radians(edge));
}

void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation,
	GLuint positionLocation, GLuint directionLocation,
	GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
	GLuint radiusLocation, GLuint edgeLocation)
{
	// Set an color uniform in the shader composed of the color component
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);

	if (isOn)
	{
		glUniform1f(ambientIntensityLocation, ambientIntensity);

		// Attach diffuse info to uniforms
		glUniform1f(diffuseIntensityLocation, diffuseIntensity);
	}
	else
	{
		glUniform1f(ambientIntensityLocation, 0.0f);
		glUniform1f(diffuseIntensityLocation, 0.0f);
	}

	// Attach Spot Light Values
	glUniform3f(positionLocation, position.x, position.y, position.z);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(radiusLocation, radius);
	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);
	glUniform1f(edgeLocation, procEdge);
}

void SpotLight::SetFlash(glm::vec3 pos, glm::vec3 dir)
{
	position = pos;
	direction = dir;
}

SpotLight::~SpotLight()
{
}
